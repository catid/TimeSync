#!/usr/bin/env python3
import argparse
import csv
import re
import statistics
from collections import defaultdict


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def infer_step_seconds(name):
    if "300s" in name:
        return 300.0
    if "120s" in name:
        return 120.0
    if re.search(r"(^|_)1s(_|$)", name):
        return 1.0
    if "small" in name:
        return 2.0
    if "large" in name:
        return 30.0
    match = re.search(r"(\\d+)s", name)
    if match:
        return float(match.group(1))
    return None


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def main():
    ap = argparse.ArgumentParser(description="Summarize step recovery vs step size.")
    ap.add_argument("csv_path", help="Merged peer_bench CSV")
    ap.add_argument("--out", default="step_size_summary.md", help="Output markdown path")
    args = ap.parse_args()

    grouped = defaultdict(list)
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            scenario = row.get("scenario", "")
            if "clock_step" not in scenario:
                continue
            step_s = infer_step_seconds(scenario)
            if step_s is None:
                continue
            method = row.get("method", "")
            rec_ab = to_float(row, "step_recover_ab_s", -1.0)
            rec_ba = to_float(row, "step_recover_ba_s", -1.0)
            if rec_ab < 0.0 and rec_ba < 0.0:
                continue
            rec = max(rec_ab, rec_ba)
            grouped[(method, step_s)].append(rec)

    methods = sorted({k[0] for k in grouped})
    steps = sorted({k[1] for k in grouped})

    lines = []
    lines.append("# Step-size sensitivity summary")
    lines.append("")
    header = ["Method"] + [f"{s:g}s" for s in steps]
    lines.append("| " + " | ".join(header) + " |")
    lines.append("| " + " | ".join(["---"] * len(header)) + " |")
    for method in methods:
        row = [method]
        for step in steps:
            vals = grouped.get((method, step), [])
            row.append(f"{median(vals):.2f}")
        lines.append("| " + " | ".join(row) + " |")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
