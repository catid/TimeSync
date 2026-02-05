#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


MAX_ERR_US = 1.0e15


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


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


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


def main():
    ap = argparse.ArgumentParser(description="Compare methods across E0-E7 scenarios.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="method_compare.md", help="Output markdown path")
    ap.add_argument("--baseline", default="", help="Baseline method label (exact match)")
    ap.add_argument("--tolerance", type=float, default=0.0, help="Allowed % over baseline for E0-E6")
    ap.add_argument("--compliant-only", action="store_true", help="Only include methods within baseline tolerance")
    ap.add_argument("--all-scenarios", action="store_true",
                    help="Include all scenario names (not just E0-E7)")
    ap.add_argument("--require-poll", action="store_true",
                    help="Ignore rows with poll_time_err_valid_ab/ba == 0")
    ap.add_argument("--score", action="store_true",
                    help="Include weighted score summary across scenario groups")
    ap.add_argument("--score-weights", default="",
                    help="Override score weights: E0-6=3,E7=2,video=1,steps=1")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    scenarios = [f"E{i}_" for i in range(8)]
    if args.all_scenarios:
        scenario_names = sorted({r["scenario"] for r in rows})
    else:
        scenario_names = sorted({r["scenario"] for r in rows if any(r["scenario"].startswith(s) for s in scenarios)})

    grouped = defaultdict(list)
    for r in rows:
        if not args.all_scenarios and not any(r["scenario"].startswith(s) for s in scenarios):
            continue
        if args.require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        grouped[key].append(r)

    baseline = {}
    baseline_groups = {}
    if args.baseline:
        for key, items in grouped.items():
            if key[0] == args.baseline:
                for sc in scenario_names:
                    vals = [
                        max(to_float(r, "poll_time_err_p95_ab_us"),
                            to_float(r, "poll_time_err_p95_ba_us"))
                        for r in items if r["scenario"] == sc
                    ]
                    cleaned = [sanitize_err_us(v) for v in vals]
                    baseline[sc] = median([v for v in cleaned if v is not None])
                # group medians for scoring
                groups = defaultdict(list)
                for sc in scenario_names:
                    g = scenario_group(sc)
                    if not g:
                        continue
                    val = baseline.get(sc, 0.0)
                    if val > 0.0:
                        groups[g].append(val)
                baseline_groups = {g: median(vals) for g, vals in groups.items()}
                break

    weights = parse_weights(args.score_weights)
    rows_out = []
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
            if g:
                group_vals[g].append(val)
        group_medians = {g: median(vals) for g, vals in group_vals.items()}
        overhead = median([to_float(r, "overhead_bps") for r in items])

        ok_baseline = ""
        if baseline:
            ok = True
            for sc in scenario_names:
                if not sc.startswith("E7"):
                    base = baseline.get(sc, 0.0)
                    if base <= 0.0:
                        continue
                    limit = base * (1.0 + args.tolerance / 100.0)
                    if scenario_vals.get(sc, 0.0) > limit:
                        ok = False
                        break
            ok_baseline = "yes" if ok else "no"

        score = None
        if args.score:
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

        rows_out.append((key, scenario_vals, overhead, ok_baseline, group_medians, score))

    rows_out.sort(key=lambda x: x[1].get("E7_drift", 0.0))

    lines = []
    lines.append("# Method Compare (E0-E7)")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    if args.baseline:
        lines.append(f"Baseline: {args.baseline} (tolerance {args.tolerance:.1f}%)")
    lines.append("")
    header = "| method | estimator | discipline | overhead_bps | " + " | ".join(scenario_names) + " |"
    if baseline:
        header = header[:-2] + " | within_baseline_E0_6 |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (4 + len(scenario_names) + (1 if baseline else 0))) + " |")
    for key, scenario_vals, overhead, ok, group_medians, score in rows_out:
        if baseline and args.compliant_only and ok != "yes":
            continue
        row = f"| {key[0]} | {key[1]} | {key[2]} | {overhead:.0f} | "
        row += " | ".join(f"{scenario_vals.get(sc, 0.0):.0f}" for sc in scenario_names)
        if baseline:
            row += f" | {ok} |"
        else:
            row += " |"
        lines.append(row)

    if args.score:
        lines.append("")
        lines.append("## Weighted Score Summary (lower is better)")
        if args.baseline:
            lines.append(f"Baseline ratios from: {args.baseline}")
        lines.append(f"Weights: {', '.join(f'{k}={v:g}' for k, v in weights.items())}")
        lines.append("")
        score_rows = [r for r in rows_out if r[5] is not None]
        score_rows.sort(key=lambda x: x[5])
        lines.append("| method | estimator | discipline | score | E0-6 | E7 | video | steps |")
        lines.append("| --- | --- | --- | --- | --- | --- | --- | --- |")
        for key, _, _, ok, groups, score in score_rows:
            if baseline and args.compliant_only and ok != "yes":
                continue
            lines.append(
                f"| {key[0]} | {key[1]} | {key[2]} | {score:.3f} | "
                f"{groups.get('E0-6', 0.0):.0f} | {groups.get('E7', 0.0):.0f} | "
                f"{groups.get('video', 0.0):.0f} | {groups.get('steps', 0.0):.0f} |"
            )

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
