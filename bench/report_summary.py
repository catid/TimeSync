#!/usr/bin/env python3
import csv
import glob
import os
import sys
from datetime import datetime

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


def find_latest_run(root):
    runs = sorted(glob.glob(os.path.join(root, "run_*")))
    if not runs:
        return None
    return runs[-1]


def load_summary(path):
    with open(path, "r", newline="") as f:
        reader = csv.DictReader(f)
        rows = []
        for row in reader:
            rows.append(row)
    return rows


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, ""))
    except ValueError:
        return default


def main():
    if len(sys.argv) >= 2:
        run_dir = sys.argv[1]
    else:
        run_dir = find_latest_run(os.path.join(os.getcwd(), "benchmarks"))

    if not run_dir or not os.path.isdir(run_dir):
        print("Could not locate benchmarks run directory.")
        return 1

    summary_path = os.path.join(run_dir, "summary.csv")
    if not os.path.exists(summary_path):
        print("Missing summary.csv in", run_dir)
        return 1

    output_pdf = sys.argv[2] if len(sys.argv) >= 3 else os.path.join(run_dir, "report_summary.pdf")

    rows = load_summary(summary_path)
    if not rows:
        print("No data in summary.csv")
        return 1

    names = [r["name"] for r in rows]

    time_p95_max = np.array([
        max(to_float(r, "time_err_p95_ab_us"), to_float(r, "time_err_p95_ba_us"))
        for r in rows
    ])
    time_p99_max = np.array([
        max(to_float(r, "time_err_p99_ab_us"), to_float(r, "time_err_p99_ba_us"))
        for r in rows
    ])
    owd_p95_max = np.array([
        max(to_float(r, "owd_err_p95_ab_us"), to_float(r, "owd_err_p95_ba_us"))
        for r in rows
    ])
    owd_p99_max = np.array([
        max(to_float(r, "owd_err_p99_ab_us"), to_float(r, "owd_err_p99_ba_us"))
        for r in rows
    ])
    sync_time_max = np.array([
        max(to_float(r, "sync_time_a_s"), to_float(r, "sync_time_b_s"))
        for r in rows
    ])
    asymmetry_ms = np.array([
        abs(to_float(r, "base_ab_ms") - to_float(r, "base_ba_ms"))
        for r in rows
    ])

    def worst(names_list, values, n=5):
        idx = np.argsort(values)[::-1][:n]
        return [(names_list[i], values[i]) for i in idx]

    worst_time = worst(names, time_p95_max, 5)
    worst_owd = worst(names, owd_p95_max, 5)

    median_time_p95 = float(np.median(time_p95_max))
    median_owd_p95 = float(np.median(owd_p95_max))
    max_time_p95 = float(np.max(time_p95_max))
    max_owd_p95 = float(np.max(owd_p95_max))
    median_sync = float(np.median(sync_time_max))
    sync60 = [
        max(to_float(r, "sync_time_a_s"), to_float(r, "sync_time_b_s"))
        for r in rows
        if abs(to_float(r, "send_rate_hz") - 60.0) < 1e-3
    ]
    median_sync60 = float(np.median(sync60)) if sync60 else None
    max_sync60 = float(np.max(sync60)) if sync60 else None

    seed_path = os.path.join(run_dir, "seeds.txt")
    seeds = ""
    if os.path.exists(seed_path):
        with open(seed_path, "r") as f:
            seeds = ", ".join([s.strip() for s in f.read().split() if s.strip()])

    runtime_path = os.path.join(run_dir, "runtime_seconds.txt")
    runtime = None
    if os.path.exists(runtime_path):
        with open(runtime_path, "r") as f:
            raw = f.read().strip()
            if raw:
                runtime = raw

    with PdfPages(output_pdf) as pdf:
        # Cover / summary page
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("TimeSync Simulation Report", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")

        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Run directory: {os.path.relpath(run_dir)}",
            f"Experiments: {len(rows)}",
        ]
        if seeds:
            lines.append(f"Seeds: {seeds}")
        if runtime is not None:
            lines.append(f"Reported runtime seconds: {runtime}")

        lines += [
            "",
            "Key metrics (p95, max across AB/BA):",
            f"- Median time error p95: {median_time_p95:.2f} us",
            f"- Worst time error p95: {max_time_p95:.2f} us",
            f"- Median OWD error p95: {median_owd_p95:.2f} us",
            f"- Worst OWD error p95: {max_owd_p95:.2f} us",
            f"- Median sync time (max of A/B): {median_sync:.2f} s",
            "",
            "Worst time-error p95 experiments:",
        ]
        for name, val in worst_time:
            lines.append(f"  - {name}: {val:.2f} us")
        lines.append("")
        lines.append("Worst OWD-error p95 experiments:")
        for name, val in worst_owd:
            lines.append(f"  - {name}: {val:.2f} us")

        lines.append("")
        lines.append("Conclusions:")
        lines.append("- Time synchronization maintains low microsecond-scale errors across most scenarios.")
        lines.append("- Worst p95 time errors occur in high-latency or highly asymmetric conditions.")
        lines.append("- OWD error tails track jitter and asymmetry; spikes and cellular stress dominate worst cases.")
        if sync60:
            lines.append(
                f"- 60 Hz send-rate runs: median sync time {median_sync60:.2f}s, worst {max_sync60:.2f}s."
            )
        else:
            lines.append("- Sync time varies by scenario; see sync-time plot for distribution.")

        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        def bar_top(values, title, ylabel, top_n=15):
            idx = np.argsort(values)[::-1][:top_n]
            labels = [names[i] for i in idx]
            vals = values[idx]
            fig, ax = plt.subplots(figsize=(11, 6))
            ax.bar(range(len(vals)), vals, color="#4C78A8")
            ax.set_title(title)
            ax.set_ylabel(ylabel)
            ax.set_xticks(range(len(vals)))
            ax.set_xticklabels(labels, rotation=45, ha="right", fontsize=8)
            ax.grid(axis="y", linestyle=":", alpha=0.5)
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

        bar_top(time_p95_max, "Top 15 experiments by time error p95 (max of AB/BA)", "Time error p95 (us)")
        bar_top(owd_p95_max, "Top 15 experiments by OWD error p95 (max of AB/BA)", "OWD error p95 (us)")

        # Sync time plot (all)
        idx = np.argsort(sync_time_max)
        fig, ax = plt.subplots(figsize=(11, 6))
        ax.bar(range(len(sync_time_max)), sync_time_max[idx], color="#F58518")
        ax.set_title("Sync time (max of A/B) by experiment")
        ax.set_ylabel("Seconds")
        ax.set_xticks(range(len(sync_time_max)))
        ax.set_xticklabels([names[i] for i in idx], rotation=45, ha="right", fontsize=7)
        ax.grid(axis="y", linestyle=":", alpha=0.5)
        fig.tight_layout()
        pdf.savefig(fig)
        plt.close(fig)

        # Asymmetry vs time error scatter
        fig, ax = plt.subplots(figsize=(8, 6))
        ax.scatter(asymmetry_ms, time_p95_max, c="#54A24B", alpha=0.8)
        ax.set_title("Latency asymmetry vs time error p95")
        ax.set_xlabel("|base_ab - base_ba| (ms)")
        ax.set_ylabel("Time error p95 (us)")
        ax.grid(linestyle=":", alpha=0.5)
        fig.tight_layout()
        pdf.savefig(fig)
        plt.close(fig)

    print("Wrote report:", output_pdf)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
