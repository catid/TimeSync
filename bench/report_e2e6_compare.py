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


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def main():
    ap = argparse.ArgumentParser(description="Compare methods across E2-E6 scenarios.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="e2e6_compare.md", help="Output markdown path")
    ap.add_argument("--methods", default="", help="Comma-separated method labels to include (exact match)")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    include = None
    if args.methods:
        include = {m.strip() for m in args.methods.split(",") if m.strip()}

    scenarios = [f"E{i}_" for i in range(2, 7)]
    scenario_names = sorted({r["scenario"] for r in rows if any(r["scenario"].startswith(s) for s in scenarios)})

    grouped = defaultdict(list)
    for r in rows:
        if not any(r["scenario"].startswith(s) for s in scenarios):
            continue
        if include and r["method"] not in include:
            continue
        key = (r["scenario"], r["method"])
        grouped[key].append(r)

    lines = []
    lines.append("# E2-E6 Method Comparison")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    if include:
        lines.append(f"Methods: {', '.join(sorted(include))}")
    lines.append("")
    lines.append("| scenario | method | p95_ab_us | p95_ba_us | p95_max_us | overhead_bps | rows |")
    lines.append("| --- | --- | --- | --- | --- | --- | --- |")

    for sc in scenario_names:
        methods = sorted({k[1] for k in grouped.keys() if k[0] == sc})
        for m in methods:
            items = grouped.get((sc, m), [])
            p95_ab = median([to_float(r, "poll_time_err_p95_ab_us") for r in items])
            p95_ba = median([to_float(r, "poll_time_err_p95_ba_us") for r in items])
            overhead = median([to_float(r, "overhead_bps") for r in items])
            p95_max = max(p95_ab, p95_ba)
            lines.append(
                f"| {sc} | {m} | {p95_ab:.0f} | {p95_ba:.0f} | {p95_max:.0f} | {overhead:.0f} | {len(items)} |"
            )

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
