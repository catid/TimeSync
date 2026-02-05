#!/usr/bin/env python3
import csv
import os
import sys
from collections import defaultdict
from datetime import datetime

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def time_metric(row, suffix):
    poll_key = f"poll_time_err_{suffix}"
    if poll_key in row and row.get(poll_key, "") != "":
        return to_float(row, poll_key)
    return to_float(row, f"offset_{suffix}")


def label_for(row):
    return f"{row['method']}|{row['estimator']}|{row['discipline']}"


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def main():
    if len(sys.argv) < 2:
        print("Usage: report_scenario_sensitivity.py <peer_bench.csv> [out_pdf]")
        return 1

    csv_path = sys.argv[1]
    out_pdf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(csv_path), "scenario_sensitivity.pdf")

    rows = load_rows(csv_path)
    if not rows:
        print("No rows in", csv_path)
        return 1

    scenarios = sorted({r["scenario"] for r in rows})
    methods = sorted({label_for(r) for r in rows})

    grouped = defaultdict(list)
    for r in rows:
        grouped[(r["scenario"], label_for(r))].append(r)

    matrix = np.full((len(scenarios), len(methods)), np.nan)
    for i, sc in enumerate(scenarios):
        for j, m in enumerate(methods):
            items = grouped.get((sc, m), [])
            if not items:
                continue
            vals = [max(time_metric(r, "p95_ab_us"), time_metric(r, "p95_ba_us")) for r in items]
            if not vals:
                continue
            matrix[i, j] = float(np.median(vals))

    # Normalize by best per scenario to show sensitivity
    norm = np.full_like(matrix, np.nan)
    for i in range(matrix.shape[0]):
        row = matrix[i]
        if np.all(np.isnan(row)):
            continue
        best = np.nanmin(row)
        if best <= 0.0 or np.isnan(best):
            continue
        norm[i] = row / best

    # Per-method sensitivity (std dev of log10 ratio)
    sensitivity = []
    for j, m in enumerate(methods):
        ratios = norm[:, j]
        ratios = ratios[np.isfinite(ratios)]
        if ratios.size == 0:
            sensitivity.append((m, np.nan))
            continue
        logv = np.log10(ratios)
        sensitivity.append((m, float(np.std(logv))))

    out_csv = os.path.join(os.path.dirname(csv_path), "scenario_sensitivity.csv")
    with open(out_csv, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["method", "log10_ratio_std"])
        for m, s in sensitivity:
            w.writerow([m, "" if np.isnan(s) else f"{s:.6f}"])

    with PdfPages(out_pdf) as pdf:
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("Scenario Sensitivity", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")
        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Input: {os.path.relpath(csv_path)}",
            f"Scenarios: {len(scenarios)}",
            f"Methods: {len(methods)}",
            "",
            "Heatmap shows ratio to best per scenario (lower is better).",
            "Sensitivity metric = stddev of log10(ratio) across scenarios.",
            f"Output CSV: {os.path.relpath(out_csv)}",
        ]
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        fig, ax = plt.subplots(figsize=(max(10, len(methods) * 0.5), max(6, len(scenarios) * 0.35)))
        mesh = ax.imshow(norm, cmap="viridis", aspect="auto", vmin=1.0, vmax=np.nanpercentile(norm, 90))
        ax.set_title("Scenario sensitivity (ratio to best per scenario)")
        ax.set_xlabel("Method")
        ax.set_ylabel("Scenario")
        ax.set_xticks(np.arange(len(methods)))
        ax.set_xticklabels(methods, rotation=45, ha="right", fontsize=7)
        ax.set_yticks(np.arange(len(scenarios)))
        ax.set_yticklabels(scenarios, fontsize=8)
        cbar = fig.colorbar(mesh, ax=ax, fraction=0.046, pad=0.04)
        cbar.set_label("ratio to best (p95 error)")
        fig.tight_layout()
        pdf.savefig(fig)
        plt.close(fig)

        # Sensitivity bar
        sens_sorted = sorted([(m, s) for m, s in sensitivity if np.isfinite(s)], key=lambda x: x[1])
        if sens_sorted:
            labels = [m for m, _ in sens_sorted]
            vals = [s for _, s in sens_sorted]
            fig, ax = plt.subplots(figsize=(max(10, len(labels) * 0.5), 6))
            ax.bar(np.arange(len(labels)), vals, color="#4C78A8")
            ax.set_title("Per-method scenario sensitivity (log10 ratio stddev)")
            ax.set_ylabel("stddev(log10 ratio)")
            ax.set_xticks(np.arange(len(labels)))
            ax.set_xticklabels(labels, rotation=45, ha="right", fontsize=7)
            ax.grid(axis="y", linestyle=":", alpha=0.4)
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

    print("Wrote scenario sensitivity report:", out_pdf)
    print("Wrote summary CSV:", out_csv)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
