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


def main():
    ap = argparse.ArgumentParser(description="Summarize probe-rate holdover performance.")
    ap.add_argument("csv_path", help="Merged peer_bench CSV")
    ap.add_argument("--out", default="lowrate_holdover.md", help="Output markdown path")
    args = ap.parse_args()

    with open(args.csv_path, "r", newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    grouped = defaultdict(list)
    for r in rows:
        method = r.get("method", "")
        rate = to_float(r, "probe_rate_hz", 0.0)
        err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                  to_float(r, "poll_time_err_p95_ba_us", 0.0))
        grouped[(method, rate)].append(err)

    methods = sorted({k[0] for k in grouped})
    rates = sorted({k[1] for k in grouped})

    lines = []
    lines.append("# Low-rate probe holdover summary")
    lines.append("")
    header = ["Method"] + [f"{r:g} Hz" for r in rates]
    lines.append("| " + " | ".join(header) + " |")
    lines.append("| " + " | ".join(["---"] * len(header)) + " |")
    for method in methods:
        row = [method]
        for rate in rates:
            vals = grouped.get((method, rate), [])
            row.append(f"{median(vals):.1f}")
        lines.append("| " + " | ".join(row) + " |")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
