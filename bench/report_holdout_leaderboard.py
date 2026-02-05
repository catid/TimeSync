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


def main():
    ap = argparse.ArgumentParser(description="Holdout leaderboard for peer_bench.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="holdout_leaderboard.csv",
                    help="Output CSV path")
    ap.add_argument("--require-poll", action="store_true",
                    help="Require poll_time_err_valid_ab/ba = 1")
    args = ap.parse_args()

    rows = []
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("train") != "0":
                continue
            if args.require_poll:
                if int(to_float(row, "poll_time_err_valid_ab", 0)) == 0:
                    continue
                if int(to_float(row, "poll_time_err_valid_ba", 0)) == 0:
                    continue
            rows.append(row)

    if not rows:
        print("No holdout rows in", args.csv_path)
        return 1

    grouped = {}
    for row in rows:
        key = (row.get("method", ""),
               row.get("estimator", ""),
               row.get("discipline", ""))
        grouped.setdefault(key, []).append(row)

    summary = []
    for key, items in grouped.items():
        p95 = []
        p99 = []
        overhead = []
        for r in items:
            err_p95 = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                          to_float(r, "poll_time_err_p95_ba_us", 0.0))
            err_p99 = max(to_float(r, "poll_time_err_p99_ab_us", 0.0),
                          to_float(r, "poll_time_err_p99_ba_us", 0.0))
            if err_p95 > 0.0:
                p95.append(err_p95)
            if err_p99 > 0.0:
                p99.append(err_p99)
            if to_float(r, "overhead_bps", 0.0) > 0.0:
                overhead.append(to_float(r, "overhead_bps", 0.0))
        if not p95:
            continue
        summary.append({
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "p95": median(p95),
            "p99": median(p99) if p99 else 0.0,
            "overhead_bps": median(overhead) if overhead else 0.0,
        })

    summary.sort(key=lambda x: (x["p95"], x["p99"], x["overhead_bps"]))

    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["method", "estimator", "discipline",
                    "poll_err_p95_us", "poll_err_p99_us", "overhead_bps"])
        for row in summary:
            w.writerow([row["method"], row["estimator"], row["discipline"],
                        f"{row['p95']:.3f}", f"{row['p99']:.3f}",
                        f"{row['overhead_bps']:.1f}"])
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
