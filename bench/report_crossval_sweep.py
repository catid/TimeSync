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


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def summarize(rows):
    grouped = {}
    for r in rows:
        key = (r.get("method", ""), r.get("estimator", ""), r.get("discipline", ""))
        grouped.setdefault(key, []).append(r)
    summary = {}
    for key, items in grouped.items():
        vals = []
        for r in items:
            err = max(to_float(r, "poll_time_err_p95_ab_us", 0.0),
                      to_float(r, "poll_time_err_p95_ba_us", 0.0))
            if err > 0.0:
                vals.append(err)
        if vals:
            summary[key] = median(vals)
    return summary


def main():
    ap = argparse.ArgumentParser(description="Cross-validation sweep summary.")
    ap.add_argument("train_csv", help="Train peer_bench.csv")
    ap.add_argument("holdout_csv", help="Holdout peer_bench.csv")
    ap.add_argument("--out", default="crossval_summary.csv", help="Output CSV")
    args = ap.parse_args()

    train_rows = load_rows(args.train_csv)
    hold_rows = load_rows(args.holdout_csv)
    if not train_rows or not hold_rows:
        print("Empty train/holdout input")
        return 1

    train = summarize(train_rows)
    hold = summarize(hold_rows)
    keys = sorted(set(train.keys()) | set(hold.keys()))

    rows = []
    for key in keys:
        rows.append({
            "method": key[0],
            "estimator": key[1],
            "discipline": key[2],
            "train_p95_us": train.get(key, 0.0),
            "holdout_p95_us": hold.get(key, 0.0),
        })

    rows.sort(key=lambda r: (r["holdout_p95_us"] or 0.0, r["train_p95_us"] or 0.0))

    with open(args.out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["method", "estimator", "discipline", "train_p95_us", "holdout_p95_us"])
        for r in rows:
            w.writerow([r["method"], r["estimator"], r["discipline"],
                        f"{r['train_p95_us']:.3f}", f"{r['holdout_p95_us']:.3f}"])
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
