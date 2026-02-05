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
    ap = argparse.ArgumentParser(description="Per-scenario percentile summary (p50/p95/p99).")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="scenario_percentiles.md", help="Output markdown path")
    ap.add_argument("--methods", default="", help="Comma-separated method list to include")
    ap.add_argument("--scenario-prefix", default="", help="Only include scenarios with this prefix")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    method_filter = set(m.strip() for m in args.methods.split(",") if m.strip())

    grouped = defaultdict(list)
    for r in rows:
        scenario = r.get("scenario", "")
        if args.scenario_prefix and not scenario.startswith(args.scenario_prefix):
            continue
        method = r.get("method", "")
        if method_filter and method not in method_filter:
            continue
        key = (scenario, method, r.get("estimator", ""), r.get("discipline", ""))
        grouped[key].append(r)

    lines = []
    lines.append("# Scenario Percentiles")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    if args.scenario_prefix:
        lines.append(f"Scenario prefix: {args.scenario_prefix}")
    if method_filter:
        lines.append(f"Methods: {', '.join(sorted(method_filter))}")
    lines.append("")
    lines.append("| scenario | method | estimator | discipline | p50_us | p95_us | p99_us |")
    lines.append("| --- | --- | --- | --- | --- | --- | --- |")

    for key in sorted(grouped.keys()):
        scenario, method, estimator, discipline = key
        items = grouped[key]
        p50 = median([max(to_float(r, "poll_time_err_p50_ab_us"),
                          to_float(r, "poll_time_err_p50_ba_us"))
                      for r in items])
        p95 = median([max(to_float(r, "poll_time_err_p95_ab_us"),
                          to_float(r, "poll_time_err_p95_ba_us"))
                      for r in items])
        p99 = median([max(to_float(r, "poll_time_err_p99_ab_us"),
                          to_float(r, "poll_time_err_p99_ba_us"))
                      for r in items])
        lines.append("| " + " | ".join([
            scenario,
            method,
            estimator,
            discipline,
            f"{p50:.0f}",
            f"{p95:.0f}",
            f"{p99:.0f}",
        ]) + " |")

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
