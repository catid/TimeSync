#!/usr/bin/env python3
import argparse
import csv
import math


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def main():
    ap = argparse.ArgumentParser(description="Plot drift vs jitter phase-plane for a method.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--method", default="M4_TimeSync", help="Method label to filter (exact match)")
    ap.add_argument("--out", default="phase_plane.pdf", help="Output PDF")
    ap.add_argument("--csv-out", default="phase_plane.csv", help="Output CSV")
    args = ap.parse_args()

    rows = []
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("method") != args.method:
                continue
            jitter = max(to_float(row, "short_iqr_mean_ab_us", 0.0),
                         to_float(row, "short_iqr_mean_ba_us", 0.0))
            skew = max(abs(to_float(row, "skew_p95_ab_ppm", 0.0)),
                       abs(to_float(row, "skew_p95_ba_ppm", 0.0)))
            err = max(to_float(row, "poll_time_err_p95_ab_us", 0.0),
                      to_float(row, "poll_time_err_p95_ba_us", 0.0))
            rows.append({
                "scenario": row.get("scenario", ""),
                "jitter_us": jitter,
                "skew_ppm": skew,
                "err_p95_us": err,
            })

    if not rows:
        print("No rows for method", args.method)
        return 1

    with open(args.csv_out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["scenario", "jitter_us", "skew_ppm", "err_p95_us"])
        for r in rows:
            w.writerow([r["scenario"], f"{r['jitter_us']:.3f}",
                        f"{r['skew_ppm']:.3f}", f"{r['err_p95_us']:.3f}"])

    try:
        import matplotlib.pyplot as plt
    except Exception as exc:
        print("matplotlib not available:", exc)
        print("Wrote CSV only:", args.csv_out)
        return 0

    jitter = [r["jitter_us"] for r in rows]
    skew = [r["skew_ppm"] for r in rows]
    err = [r["err_p95_us"] for r in rows]

    fig, ax = plt.subplots(figsize=(7, 5))
    sc = ax.scatter(jitter, skew, c=err, cmap="viridis", alpha=0.8)
    ax.set_xlabel("Short-window IQR mean (us)")
    ax.set_ylabel("Skew p95 (ppm)")
    ax.set_title(f"Drift vs jitter phase plane ({args.method})")
    ax.grid(linestyle=":", alpha=0.5)
    cbar = plt.colorbar(sc, ax=ax)
    cbar.set_label("Time error p95 (us)")
    fig.tight_layout()
    fig.savefig(args.out)
    plt.close(fig)

    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
