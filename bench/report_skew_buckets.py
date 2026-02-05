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


def parse_buckets(spec):
    buckets = []
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        if "-" not in part:
            continue
        lo, hi = part.split("-", 1)
        try:
            buckets.append((float(lo), float(hi)))
        except ValueError:
            continue
    return buckets


def bucket_for(value, buckets):
    for lo, hi in buckets:
        if value >= lo and value < hi:
            return (lo, hi)
    return None


def main():
    ap = argparse.ArgumentParser(description="Stratify peer_bench results by skew magnitude buckets.")
    ap.add_argument("csv", help="peer_bench.csv path")
    ap.add_argument("--out", default="skew_buckets.md", help="Output markdown path")
    ap.add_argument("--buckets", default="0-50,50-150,150-400,400-1000",
                    help="Comma-separated skew buckets in ppm")
    ap.add_argument("--top", type=int, default=10, help="Top N methods per bucket")
    args = ap.parse_args()

    buckets = parse_buckets(args.buckets)
    if not buckets:
        print("No valid buckets specified")
        return 1

    rows = []
    with open(args.csv, "r", newline="") as f:
        rows = list(csv.DictReader(f))

    grouped = {}
    for r in rows:
        skew = to_float(r, "skew_mag_ppm", 0.0)
        b = bucket_for(skew, buckets)
        if b is None:
            continue
        err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                  to_float(r, "poll_time_err_p95_ba_us", 0.0))
        key = (b, r.get("method", ""), r.get("estimator", ""), r.get("discipline", ""))
        grouped.setdefault(key, []).append(err)

    lines = ["# Skew Bucket Leaderboards", "", f"Source: {args.csv}", ""]
    for b in buckets:
        lo, hi = b
        lines.append(f"## {lo:.0f}-{hi:.0f} ppm")
        lines.append("")
        lines.append("| method | estimator | discipline | poll_p95_us | rows |")
        lines.append("| --- | --- | --- | --- | --- |")
        rows_out = []
        for key, vals in grouped.items():
            if key[0] != b:
                continue
            rows_out.append((median(vals), len(vals), key))
        rows_out.sort(key=lambda x: x[0])
        for med, count, key in rows_out[: args.top]:
            _, method, est, disc = key
            lines.append(f"| {method} | {est} | {disc} | {med:.0f} | {count} |")
        lines.append("")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
