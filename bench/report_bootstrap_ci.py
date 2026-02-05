#!/usr/bin/env python3
import argparse
import csv
import random
import statistics
from collections import defaultdict


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def bootstrap_ci(values, samples=1000, alpha=0.05, rng=None):
    if rng is None:
        rng = random.Random(0)
    n = len(values)
    if n == 0:
        return (0.0, 0.0)
    meds = []
    for _ in range(samples):
        draw = [values[rng.randrange(n)] for _ in range(n)]
        meds.append(statistics.median(draw))
    meds.sort()
    lo_idx = int((alpha / 2) * (samples - 1))
    hi_idx = int((1 - alpha / 2) * (samples - 1))
    return meds[lo_idx], meds[hi_idx]


def main():
    ap = argparse.ArgumentParser(description="Bootstrap CI for scenario medians.")
    ap.add_argument("csv", help="peer_bench.csv path")
    ap.add_argument("--out", default="bootstrap_ci.md", help="Output markdown path")
    ap.add_argument("--boots", type=int, default=1000, help="Bootstrap samples")
    ap.add_argument("--alpha", type=float, default=0.05, help="Alpha for CI")
    ap.add_argument("--top", type=int, default=10, help="Top N per scenario")
    args = ap.parse_args()

    rows = []
    with open(args.csv, "r", newline="") as f:
        rows = list(csv.DictReader(f))

    grouped = defaultdict(list)
    for r in rows:
        err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                  to_float(r, "poll_time_err_p95_ba_us", 0.0))
        key = (r.get("scenario", ""), r.get("method", ""), r.get("estimator", ""), r.get("discipline", ""))
        grouped[key].append(err)

    scenarios = sorted({k[0] for k in grouped.keys()})
    lines = ["# Bootstrap CI (median poll_time_err_p95)", "", f"Source: {args.csv}", ""]
    rng = random.Random(0)
    for scenario in scenarios:
        rows_out = []
        for key, vals in grouped.items():
            if key[0] != scenario:
                continue
            vals = [v for v in vals if v > 0.0]
            if not vals:
                continue
            med = statistics.median(vals)
            lo, hi = bootstrap_ci(vals, samples=args.boots, alpha=args.alpha, rng=rng)
            rows_out.append((med, lo, hi, len(vals), key))
        rows_out.sort(key=lambda x: x[0])
        lines.append(f"## {scenario}")
        lines.append("")
        lines.append("| method | estimator | discipline | median | ci_low | ci_high | rows |")
        lines.append("| --- | --- | --- | --- | --- | --- | --- |")
        for med, lo, hi, count, key in rows_out[: args.top]:
            lines.append(
                f"| {key[1]} | {key[2]} | {key[3]} | {med:.0f} | {lo:.0f} | {hi:.0f} | {count} |"
            )
        lines.append("")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
