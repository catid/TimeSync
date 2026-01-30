#!/usr/bin/env python3
import csv
import os
import sys
from datetime import datetime

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


THRESH_TIME_P95_US = 5000.0
THRESH_TIME_P99_US = 10000.0
THRESH_OWD_P95_US = 5000.0
THRESH_SYNC_S = 5.0

THRESH_TIME_P95_OK_US = 10000.0
THRESH_TIME_P99_OK_US = 20000.0
THRESH_OWD_P95_OK_US = 10000.0


def get_float(row, key, default=0.0):
    try:
        return float(row.get(key, ""))
    except ValueError:
        return default


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def metric_arrays(rows):
    time_p95 = []
    time_p99 = []
    owd_p95 = []
    owd_p99 = []
    sync = []

    for row in rows:
        time_p95.append(max(get_float(row, "time_err_p95_ab_us"), get_float(row, "time_err_p95_ba_us")))
        time_p99.append(max(get_float(row, "time_err_p99_ab_us"), get_float(row, "time_err_p99_ba_us")))
        owd_p95.append(max(get_float(row, "owd_err_p95_ab_us"), get_float(row, "owd_err_p95_ba_us")))
        owd_p99.append(max(get_float(row, "owd_err_p99_ab_us"), get_float(row, "owd_err_p99_ba_us")))
        sync.append(max(get_float(row, "sync_time_a_s"), get_float(row, "sync_time_b_s")))

    return {
        "time_p95": np.array(time_p95),
        "time_p99": np.array(time_p99),
        "owd_p95": np.array(owd_p95),
        "owd_p99": np.array(owd_p99),
        "sync": np.array(sync),
    }


def heatmap(ax, x, y, xlabel, ylabel, title, bins=40, xlim=None, ylim=None, thresh_lines=None):
    ax.hist2d(x, y, bins=bins, cmap="viridis")
    if xlim:
        ax.set_xlim(xlim)
    if ylim:
        ax.set_ylim(ylim)
    if thresh_lines:
        for line in thresh_lines:
            axis = line[0]
            value = line[1]
            color = line[2] if len(line) > 2 else "#D62728"
            if axis == "x":
                ax.axvline(value, color=color, linestyle="--", linewidth=1)
            else:
                ax.axhline(value, color=color, linestyle="--", linewidth=1)
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(linestyle=":", alpha=0.3)


def main():
    if len(sys.argv) < 2:
        print("Usage: report_montecarlo.py <montecarlo.csv> [out_pdf]")
        return 1

    csv_path = sys.argv[1]
    out_pdf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(csv_path), "montecarlo_report.pdf")

    rows = load_rows(csv_path)
    if not rows:
        print("No rows in", csv_path)
        return 1

    metrics = metric_arrays(rows)

    good_mask = (
        (metrics["time_p95"] <= THRESH_TIME_P95_US)
        & (metrics["time_p99"] <= THRESH_TIME_P99_US)
        & (metrics["owd_p95"] <= THRESH_OWD_P95_US)
        & (metrics["sync"] <= THRESH_SYNC_S)
    )
    good_rate = float(np.mean(good_mask)) * 100.0

    ok_mask = (
        (metrics["time_p95"] <= THRESH_TIME_P95_OK_US)
        & (metrics["time_p99"] <= THRESH_TIME_P99_OK_US)
        & (metrics["owd_p95"] <= THRESH_OWD_P95_OK_US)
        & (metrics["sync"] <= THRESH_SYNC_S)
    )
    ok_rate = float(np.mean(ok_mask)) * 100.0

    with PdfPages(out_pdf) as pdf:
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("TimeSync Monte Carlo Heatmap Report", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")

        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Samples: {len(rows)}",
            "",
            "Good region thresholds:",
            f"- time_p95 <= {THRESH_TIME_P95_US:.0f} us",
            f"- time_p99 <= {THRESH_TIME_P99_US:.0f} us",
            f"- owd_p95 <= {THRESH_OWD_P95_US:.0f} us",
            f"- sync_time <= {THRESH_SYNC_S:.1f} s",
            "",
            "Acceptable region thresholds:",
            f"- time_p95 <= {THRESH_TIME_P95_OK_US:.0f} us",
            f"- time_p99 <= {THRESH_TIME_P99_OK_US:.0f} us",
            f"- owd_p95 <= {THRESH_OWD_P95_OK_US:.0f} us",
            f"- sync_time <= {THRESH_SYNC_S:.1f} s",
            "",
            f"Good-region coverage: {good_rate:.1f}%",
            f"Acceptable-region coverage: {ok_rate:.1f}%",
            "",
            "Interpretation:",
            "- Heatmaps show density over metric pairs.",
            "- Red dashed lines mark good thresholds; orange dashed mark acceptable thresholds.",
        ]

        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        # time_p95 vs owd_p95
        fig, ax = plt.subplots(figsize=(8.5, 6.5))
        heatmap(
            ax,
            metrics["time_p95"],
            metrics["owd_p95"],
            "Time error p95 (us)",
            "OWD error p95 (us)",
            "Time p95 vs OWD p95",
            thresh_lines=[
                ("x", THRESH_TIME_P95_US, "#D62728"),
                ("x", THRESH_TIME_P95_OK_US, "#F58518"),
                ("y", THRESH_OWD_P95_US, "#D62728"),
                ("y", THRESH_OWD_P95_OK_US, "#F58518"),
            ],
        )
        pdf.savefig(fig)
        plt.close(fig)

        # time_p95 vs sync
        fig, ax = plt.subplots(figsize=(8.5, 6.5))
        heatmap(
            ax,
            metrics["time_p95"],
            metrics["sync"],
            "Time error p95 (us)",
            "Sync time (s)",
            "Time p95 vs Sync time",
            thresh_lines=[
                ("x", THRESH_TIME_P95_US, "#D62728"),
                ("x", THRESH_TIME_P95_OK_US, "#F58518"),
                ("y", THRESH_SYNC_S, "#D62728"),
            ],
        )
        pdf.savefig(fig)
        plt.close(fig)

        # owd_p95 vs sync
        fig, ax = plt.subplots(figsize=(8.5, 6.5))
        heatmap(
            ax,
            metrics["owd_p95"],
            metrics["sync"],
            "OWD error p95 (us)",
            "Sync time (s)",
            "OWD p95 vs Sync time",
            thresh_lines=[
                ("x", THRESH_OWD_P95_US, "#D62728"),
                ("x", THRESH_OWD_P95_OK_US, "#F58518"),
                ("y", THRESH_SYNC_S, "#D62728"),
            ],
        )
        pdf.savefig(fig)
        plt.close(fig)

        # time_p99 vs owd_p99
        fig, ax = plt.subplots(figsize=(8.5, 6.5))
        heatmap(
            ax,
            metrics["time_p99"],
            metrics["owd_p99"],
            "Time error p99 (us)",
            "OWD error p99 (us)",
            "Time p99 vs OWD p99",
            thresh_lines=[
                ("x", THRESH_TIME_P99_US, "#D62728"),
                ("x", THRESH_TIME_P99_OK_US, "#F58518"),
                ("y", THRESH_TIME_P99_US, "#D62728"),
                ("y", THRESH_TIME_P99_OK_US, "#F58518"),
            ],
        )
        pdf.savefig(fig)
        plt.close(fig)

    print("Wrote Monte Carlo report:", out_pdf)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
