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
    ap = argparse.ArgumentParser(description="Per-scenario delta vs baseline.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method name")
    ap.add_argument("--out", default="scenario_delta_vs_baseline.csv",
                    help="Output CSV path")
    args = ap.parse_args()

    with open(args.csv_path, "r", newline="") as f:
        rows = list(csv.DictReader(f))
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    baseline = {}
    grouped = {}
    for r in rows:
        scenario = r.get("scenario", "")
        method = r.get("method", "")
        key = (scenario, method, r.get("estimator", ""), r.get("discipline", ""))
        grouped.setdefault(key, []).append(r)
        if method == args.baseline:
            baseline.setdefault(scenario, []).append(r)

    base_vals = {}
    for scenario, items in baseline.items():
        p95 = []
        p99 = []
        for r in items:
            p95.append(max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                           to_float(r, "poll_time_err_p95_ba_us", 0.0)))
            p99.append(max(to_float(r, "poll_time_err_p99_ab_us", 0.0),
                           to_float(r, "poll_time_err_p99_ba_us", 0.0)))
        base_vals[scenario] = {
            "p95": median([v for v in p95 if v > 0.0]),
            "p99": median([v for v in p99 if v > 0.0]),
        }

    out_rows = []
    for key, items in grouped.items():
        scenario, method, estimator, discipline = key
        base = base_vals.get(scenario)
        if not base:
            continue
        p95 = []
        p99 = []
        for r in items:
            p95.append(max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                           to_float(r, "poll_time_err_p95_ba_us", 0.0)))
            p99.append(max(to_float(r, "poll_time_err_p99_ab_us", 0.0),
                           to_float(r, "poll_time_err_p99_ba_us", 0.0)))
        p95_med = median([v for v in p95 if v > 0.0])
        p99_med = median([v for v in p99 if v > 0.0])
        if p95_med <= 0.0:
            continue
        out_rows.append({
            "scenario": scenario,
            "method": method,
            "estimator": estimator,
            "discipline": discipline,
            "p95_us": p95_med,
            "p99_us": p99_med,
            "delta_p95_us": p95_med - base["p95"],
            "delta_p99_us": p99_med - base["p99"],
        })

    out_rows.sort(key=lambda r: (r["scenario"], r["delta_p95_us"]))

    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["scenario", "method", "estimator", "discipline",
                    "p95_us", "p99_us", "delta_p95_us", "delta_p99_us"])
        for r in out_rows:
            w.writerow([r["scenario"], r["method"], r["estimator"], r["discipline"],
                        f"{r['p95_us']:.3f}", f"{r['p99_us']:.3f}",
                        f"{r['delta_p95_us']:.3f}", f"{r['delta_p99_us']:.3f}"])
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
