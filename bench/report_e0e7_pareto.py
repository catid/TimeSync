#!/usr/bin/env python3
import argparse
import csv
import statistics


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def scenario_group(name):
    if name.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_")):
        return "E0-6"
    if name.startswith("E7_"):
        return "E7"
    return None


def dominates(a, b):
    return (a["E0-6"] <= b["E0-6"] and a["E7"] <= b["E7"]
            and (a["E0-6"] < b["E0-6"] or a["E7"] < b["E7"]))


def main():
    ap = argparse.ArgumentParser(description="Pareto frontier for E0–E6 vs E7.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="pareto_e0e7.md", help="Output markdown path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method label")
    ap.add_argument("--tolerance", type=float, default=0.0,
                    help="Allowed % over baseline for E0–E6")
    ap.add_argument("--require-poll", action="store_true",
                    help="Require poll_time_err_valid_ab/ba = 1")
    args = ap.parse_args()

    with open(args.csv_path, "r", newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    grouped = {}
    for r in rows:
        g = scenario_group(r["scenario"])
        if g is None:
            continue
        if args.require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        grouped.setdefault(key, []).append(r)

    baseline_vals = {}
    for key, items in grouped.items():
        if key[0] != args.baseline:
            continue
        vals = {"E0-6": [], "E7": []}
        for r in items:
            g = scenario_group(r["scenario"])
            if not g:
                continue
            err = max(to_float(r, "poll_time_err_p95_ab_us"),
                      to_float(r, "poll_time_err_p95_ba_us"))
            if err > 0.0:
                vals[g].append(err)
        baseline_vals = {k: median(v) for k, v in vals.items()}
        break

    entries = []
    for key, items in grouped.items():
        vals = {"E0-6": [], "E7": []}
        for r in items:
            g = scenario_group(r["scenario"])
            if not g:
                continue
            err = max(to_float(r, "poll_time_err_p95_ab_us"),
                      to_float(r, "poll_time_err_p95_ba_us"))
            if err > 0.0:
                vals[g].append(err)
        e0 = median(vals["E0-6"])
        e7 = median(vals["E7"])
        if e0 <= 0.0 or e7 <= 0.0:
            continue
        safe = True
        if baseline_vals.get("E0-6", 0.0) > 0.0:
            limit = baseline_vals["E0-6"] * (1.0 + args.tolerance / 100.0)
            safe = e0 <= limit
        entries.append({
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "E0-6": e0,
            "E7": e7,
            "safe": safe,
        })

    frontier = []
    for a in entries:
        if any(dominates(b, a) for b in entries):
            continue
        frontier.append(a)

    frontier.sort(key=lambda x: (x["E0-6"], x["E7"]))

    lines = []
    lines.append("# E0–E6 vs E7 Pareto frontier")
    lines.append("")
    lines.append(f"Baseline: `{args.baseline}`, tolerance {args.tolerance:.1f}%")
    lines.append("")
    lines.append("| Method | E0-6 p95 | E7 p95 | Baseline-safe |")
    lines.append("| --- | ---: | ---: | --- |")
    for e in frontier:
        lines.append(f"| {e['method']} | {e['E0-6']:.2f} | {e['E7']:.2f} | {'Y' if e['safe'] else 'N'} |")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
