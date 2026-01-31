#!/usr/bin/env python3
import csv
import glob
import os
import sys
from datetime import datetime

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

GOOD_P95 = 5000.0
GOOD_P99 = 10000.0
GOOD_OWD = 5000.0
GOOD_SYNC = 5.0

OK_P95 = 10000.0
OK_P99 = 20000.0
OK_OWD = 10000.0


def get_float(row, key, default=0.0):
    try:
        return float(row.get(key, ""))
    except ValueError:
        return default


def time_metric(row, suffix):
    poll_key = f"poll_time_err_{suffix}"
    if poll_key in row and row.get(poll_key, "") != "":
        return get_float(row, poll_key)
    return get_float(row, f"time_err_{suffix}")


def axis_value(row, axis):
    if axis == "jitter_ms":
        return (get_float(row, "jitter_ab_ms") + get_float(row, "jitter_ba_ms")) * 0.5
    if axis == "loss":
        return get_float(row, "loss_ab")
    if axis == "asymmetry_ms":
        return abs(get_float(row, "base_ab_ms") - get_float(row, "base_ba_ms"))
    if axis == "sync_s":
        return get_float(row, "sync_interval_s")
    if axis == "rate_hz":
        return get_float(row, "send_rate_hz")
    if axis == "latency_ms":
        return (get_float(row, "base_ab_ms") + get_float(row, "base_ba_ms")) * 0.5
    return 0.0


def classify(row):
    time_p95 = max(time_metric(row, "p95_ab_us"), time_metric(row, "p95_ba_us"))
    time_p99 = max(time_metric(row, "p99_ab_us"), time_metric(row, "p99_ba_us"))
    owd_p95 = max(get_float(row, "owd_err_p95_ab_us"), get_float(row, "owd_err_p95_ba_us"))
    sync = max(get_float(row, "sync_time_a_s"), get_float(row, "sync_time_b_s"))

    if time_p95 <= GOOD_P95 and time_p99 <= GOOD_P99 and owd_p95 <= GOOD_OWD and sync <= GOOD_SYNC:
        return 2
    if time_p95 <= OK_P95 and time_p99 <= OK_P99 and owd_p95 <= OK_OWD and sync <= GOOD_SYNC:
        return 1
    return 0


def heatmap_score(ax, x, y, score, xlabel, ylabel, title, bins=40):
    counts, xedges, yedges = np.histogram2d(x, y, bins=bins)
    score_sum, _, _ = np.histogram2d(x, y, bins=[xedges, yedges], weights=score)
    with np.errstate(divide='ignore', invalid='ignore'):
        avg = np.divide(score_sum, counts, out=np.zeros_like(score_sum), where=counts > 0)

    mesh = ax.pcolormesh(xedges, yedges, avg.T, cmap="RdYlGn", vmin=0.0, vmax=2.0)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(linestyle=":", alpha=0.3)
    return mesh


def main():
    if len(sys.argv) < 2:
        print("Usage: report_drift_heatmaps.py <run_dir> [out_pdf]")
        return 1

    run_dir = sys.argv[1]
    out_pdf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(run_dir, "drift_heatmaps.pdf")

    csvs = sorted(glob.glob(os.path.join(run_dir, "dense_*.csv")))
    if not csvs:
        print("No dense_*.csv found in", run_dir)
        return 1

    with PdfPages(out_pdf) as pdf:
        # cover
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("Drift vs Other Metrics Heatmaps", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")
        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Run directory: {os.path.relpath(run_dir)}",
            "",
            "Score legend (colorbar):",
            "- 2.0 = Good (<=5ms p95, <=10ms p99, <=5ms OWD p95, sync<=5s)",
            "- 1.0 = Acceptable (<=10ms p95, <=20ms p99, <=10ms OWD p95, sync<=5s)",
            "- 0.0 = Fail",
        ]
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        for path in csvs:
            axis = os.path.basename(path).replace("dense_", "").replace(".csv", "")
            rows = list(csv.DictReader(open(path)))
            if not rows:
                continue

            drift = [abs(get_float(r, "drift_a_ppm")) for r in rows]
            axis_vals = [axis_value(r, axis) for r in rows]
            scores = [classify(r) for r in rows]

            fig, ax = plt.subplots(figsize=(8.5, 6.5))
            mesh = heatmap_score(
                ax,
                drift,
                axis_vals,
                scores,
                "Drift (ppm)",
                axis,
                f"Drift vs {axis} (performance score)",
                bins=50,
            )
            cbar = fig.colorbar(mesh, ax=ax, fraction=0.046, pad=0.04)
            cbar.set_ticks([0.0, 1.0, 2.0])
            cbar.set_ticklabels(["Fail", "Acceptable", "Good"])
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

    print("Wrote drift heatmap report:", out_pdf)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
