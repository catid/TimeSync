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


def main():
    ap = argparse.ArgumentParser(description="Summarize GSP guard/IQR sweep.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="gsp_guard_sweep.csv",
                    help="Output CSV path")
    ap.add_argument("--method", default="M4_TimeSyncShadow",
                    help="Method label filter")
    args = ap.parse_args()

    rows = []
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("method") != args.method:
                continue
            rows.append(row)

    if not rows:
        print("No rows for method", args.method)
        return 1

    grouped = {}
    for row in rows:
        key = (row.get("method", ""),
               row.get("estimator", ""),
               row.get("discipline", ""))
        grouped.setdefault(key, []).append(row)

    summary = []
    for key, items in grouped.items():
        e06 = []
        e7 = []
        for r in items:
            grp = scenario_group(r.get("scenario", ""))
            if grp is None:
                continue
            err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                      to_float(r, "poll_time_err_p95_ba_us", 0.0))
            if err <= 0.0:
                continue
            if grp == "E0-6":
                e06.append(err)
            elif grp == "E7":
                e7.append(err)
        if not e06 or not e7:
            continue
        e06_med = median(e06)
        e7_med = median(e7)
        summary.append({
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "e06_p95_us": e06_med,
            "e7_p95_us": e7_med,
            "score": e06_med + e7_med,
        })

    summary.sort(key=lambda x: x["score"])

    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["method", "estimator", "discipline",
                    "e06_p95_us", "e7_p95_us", "score"])
        for row in summary:
            w.writerow([row["method"], row["estimator"], row["discipline"],
                        f"{row['e06_p95_us']:.3f}",
                        f"{row['e7_p95_us']:.3f}",
                        f"{row['score']:.3f}"])
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
