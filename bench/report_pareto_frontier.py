#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict


MAX_ERR_US = 1.0e15


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def sanitize_err_us(value):
    if value <= 0.0:
        return None
    if value > MAX_ERR_US:
        return None
    return value


def scenario_group(name):
    if name.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_")):
        return "E0-6"
    if name.startswith("E7_"):
        return "E7"
    if "video" in name:
        return "video"
    if ("step" in name) or ("clock" in name) or ("jump" in name):
        return "steps"
    return None


def parse_weights(spec):
    weights = {"E0-6": 3.0, "E7": 2.0, "video": 1.0, "steps": 1.0}
    if not spec:
        return weights
    for part in spec.split(","):
        if not part.strip():
            continue
        if "=" not in part:
            continue
        key, val = part.split("=", 1)
        key = key.strip()
        try:
            weights[key] = float(val.strip())
        except ValueError:
            continue
    return weights


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def dominates(a, b, dims):
    better_or_equal = True
    strictly_better = False
    for d in dims:
        av = a.get(d, 0.0)
        bv = b.get(d, 0.0)
        if av <= 0.0 or bv <= 0.0:
            return False
        if av > bv:
            better_or_equal = False
            break
        if av < bv:
            strictly_better = True
    return better_or_equal and strictly_better


def main():
    ap = argparse.ArgumentParser(description="Compute Pareto frontier across scenario groups.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="pareto_frontier.md", help="Output markdown path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method label")
    ap.add_argument("--tolerance", type=float, default=0.0, help="Allowed % over baseline for E0-6")
    ap.add_argument("--weights", default="", help="Composite score weights")
    ap.add_argument("--groups", default="E0-6,E7,video,steps",
                    help="Comma list of groups to consider for Pareto")
    ap.add_argument("--require-poll", action="store_true",
                    help="Require poll_time_err_valid_ab/ba = 1 for all rows")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    scenario_names = sorted({r["scenario"] for r in rows})
    groups_req = [g.strip() for g in args.groups.split(",") if g.strip()]
    weights = parse_weights(args.weights)

    grouped = defaultdict(list)
    for r in rows:
        if args.require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        grouped[key].append(r)

    baseline_scenario = {}
    baseline_groups = {}
    for key, items in grouped.items():
        if key[0] != args.baseline:
            continue
        for sc in scenario_names:
            vals = [
                max(to_float(r, "poll_time_err_p95_ab_us"),
                    to_float(r, "poll_time_err_p95_ba_us"))
                for r in items if r["scenario"] == sc
            ]
            cleaned = [sanitize_err_us(v) for v in vals]
            baseline_scenario[sc] = median([v for v in cleaned if v is not None])
        group_vals = defaultdict(list)
        for sc, val in baseline_scenario.items():
            g = scenario_group(sc)
            if g and val > 0.0:
                group_vals[g].append(val)
        baseline_groups = {g: median(vals) for g, vals in group_vals.items()}
        break

    entries = []
    for key, items in grouped.items():
        scenario_vals = {}
        for sc in scenario_names:
            vals = [
                max(to_float(r, "poll_time_err_p95_ab_us"),
                    to_float(r, "poll_time_err_p95_ba_us"))
                for r in items if r["scenario"] == sc
            ]
            cleaned = [sanitize_err_us(v) for v in vals]
            scenario_vals[sc] = median([v for v in cleaned if v is not None])
        group_vals = defaultdict(list)
        for sc, val in scenario_vals.items():
            g = scenario_group(sc)
            if g and val > 0.0:
                group_vals[g].append(val)
        group_medians = {g: median(vals) for g, vals in group_vals.items()}

        ok_baseline = True
        for sc, base in baseline_scenario.items():
            if not sc.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_")):
                continue
            if base <= 0.0:
                continue
            limit = base * (1.0 + args.tolerance / 100.0)
            if scenario_vals.get(sc, 0.0) > limit:
                ok_baseline = False
                break

        overhead = median([to_float(r, "overhead_bps") for r in items])
        score = None
        if all(group_medians.get(g, 0.0) > 0.0 for g in groups_req):
            total = 0.0
            weight_sum = 0.0
            for g, w in weights.items():
                if g not in group_medians:
                    continue
                denom = baseline_groups.get(g, 0.0)
                val = group_medians[g]
                ratio = val / denom if denom > 0.0 else val
                total += w * ratio
                weight_sum += w
            score = total / weight_sum if weight_sum > 0 else None

        entry = {
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "overhead_bps": overhead,
            "baseline_safe": ok_baseline,
            "score": score,
        }
        entry.update(group_medians)
        entries.append(entry)

    dims = [g for g in groups_req if g in {"E0-6", "E7", "video", "steps"}]
    frontier = []
    for i, a in enumerate(entries):
        dominated = False
        for j, b in enumerate(entries):
            if i == j:
                continue
            if dominates(b, a, dims):
                dominated = True
                break
        if not dominated:
            frontier.append(a)

    frontier.sort(key=lambda x: (x.get("E7", 0.0), x.get("E0-6", 0.0)))

    lines = []
    lines.append("# Pareto Frontier")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    lines.append(f"Groups: {', '.join(dims)}")
    lines.append("")
    lines.append("| method | estimator | discipline | E0-6 | E7 | video | steps | overhead_bps | baseline_safe | score |")
    lines.append("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |")
    for e in frontier:
        lines.append("| " + " | ".join([
            e["method"],
            e["estimator"],
            e["discipline"],
            f"{e.get('E0-6', 0.0):.0f}",
            f"{e.get('E7', 0.0):.0f}",
            f"{e.get('video', 0.0):.0f}",
            f"{e.get('steps', 0.0):.0f}",
            f"{e.get('overhead_bps', 0.0):.0f}",
            "yes" if e.get("baseline_safe") else "no",
            f"{e['score']:.3f}" if e.get("score") is not None else "",
        ]) + " |")

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
