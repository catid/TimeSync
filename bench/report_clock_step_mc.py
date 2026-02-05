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


def percentile(values, q):
    if not values:
        return 0.0
    values = sorted(values)
    if q <= 0:
        return values[0]
    if q >= 1:
        return values[-1]
    idx = int(round(q * (len(values) - 1)))
    return values[idx]


def main():
    ap = argparse.ArgumentParser(description="Summarize clock-step Monte Carlo distribution")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--scenario", default="E26_clock_step_mc", help="Scenario name")
    ap.add_argument("--out", default="clock_step_mc.md", help="Output markdown")
    args = ap.parse_args()

    per_method = defaultdict(list)
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("scenario") != args.scenario:
                continue
            method = row.get("method", "")
            step_ab = to_float(row, "step_recover_ab_s", 0.0)
            step_ba = to_float(row, "step_recover_ba_s", 0.0)
            poll_p95 = max(
                to_float(row, "poll_time_err_p95_ab_us", 0.0),
                to_float(row, "poll_time_err_p95_ba_us", 0.0),
            )
            per_method[method].append((step_ab, step_ba, poll_p95))

    lines = []
    lines.append("# Clock-Step Monte Carlo Summary\n")
    lines.append(f"Scenario: {args.scenario}\n")
    lines.append(f"Source: {args.csv_path}\n\n")
    lines.append("| method | samples | no_recover | step_p50_s | step_p95_s | poll_p95_med_us | poll_p95_p95_us |\n")
    lines.append("| --- | --- | --- | --- | --- | --- | --- |\n")

    for method in sorted(per_method.keys()):
        rows = per_method[method]
        steps = []
        no_recover = 0
        poll_vals = []
        for step_ab, step_ba, poll in rows:
            step = max(step_ab, step_ba)
            if step < 0:
                no_recover += 1
            else:
                steps.append(step)
            poll_vals.append(poll)
        samples = len(rows)
        step_p50 = percentile(steps, 0.50) if steps else 0.0
        step_p95 = percentile(steps, 0.95) if steps else 0.0
        poll_p50 = percentile(poll_vals, 0.50) if poll_vals else 0.0
        poll_p95 = percentile(poll_vals, 0.95) if poll_vals else 0.0
        lines.append(
            f"| {method} | {samples} | {no_recover} | {step_p50:.3f} | {step_p95:.3f} | {poll_p50:.1f} | {poll_p95:.1f} |\n"
        )

    with open(args.out, "w", newline="") as f:
        f.writelines(lines)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
