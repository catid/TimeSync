#!/usr/bin/env python3
import argparse
import csv
import os
import sys
from collections import defaultdict
from datetime import datetime

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

MAX_RECOVER_S = 1.0e6


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def recover_seconds(row, key_base):
    key_us = f"{key_base}_ab_us"
    key_us_ba = f"{key_base}_ba_us"
    key_s = f"{key_base}_ab_s"
    key_s_ba = f"{key_base}_ba_s"
    if key_us in row and row.get(key_us, "") not in (None, ""):
        ab = to_float(row, key_us, 0.0) / 1_000_000.0
        ba = to_float(row, key_us_ba, 0.0) / 1_000_000.0
        return max(ab, ba)
    ab = to_float(row, key_s, 0.0)
    ba = to_float(row, key_s_ba, 0.0)
    return max(ab, ba)


def sanitize(values):
    cleaned = []
    for v in values:
        if v <= 0.0:
            continue
        if v > MAX_RECOVER_S:
            continue
        cleaned.append(v)
    return cleaned


def label_for(row):
    return f"{row['method']}|{row['estimator']}|{row['discipline']}"


def main():
    parser = argparse.ArgumentParser(description="Recovery time distribution report")
    parser.add_argument("csv", help="peer_bench.csv")
    parser.add_argument("--metric", default="step_recover5", help="Base metric (e.g. step_recover5)")
    parser.add_argument("--out", default=None, help="Output PDF path")
    args = parser.parse_args()

    csv_path = args.csv
    out_pdf = args.out or os.path.join(os.path.dirname(csv_path), f"recovery_dist_{args.metric}.pdf")

    with open(csv_path, "r", newline="") as f:
        rows = list(csv.DictReader(f))

    if not rows:
        print("No rows in", csv_path)
        return 1

    grouped = defaultdict(list)
    for row in rows:
        grouped[label_for(row)].append(recover_seconds(row, args.metric))

    summary_rows = []
    for key, values in grouped.items():
        cleaned = sanitize(values)
        if not cleaned:
            continue
        arr = np.array(cleaned)
        summary_rows.append({
            "method": key,
            "count": len(arr),
            "mean_s": float(np.mean(arr)),
            "p50_s": float(np.percentile(arr, 50)),
            "p90_s": float(np.percentile(arr, 90)),
            "p95_s": float(np.percentile(arr, 95)),
            "p99_s": float(np.percentile(arr, 99)),
        })

    summary_rows.sort(key=lambda r: r["p50_s"])

    out_csv = os.path.join(os.path.dirname(csv_path), f"recovery_dist_{args.metric}.csv")
    with open(out_csv, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(summary_rows[0].keys()) if summary_rows else [])
        if summary_rows:
            writer.writeheader()
            for row in summary_rows:
                writer.writerow(row)

    with PdfPages(out_pdf) as pdf:
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("Recovery Time Distribution", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")
        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Input: {os.path.relpath(csv_path)}",
            f"Metric: {args.metric}",
            f"Methods: {len(summary_rows)}",
            f"Output CSV: {os.path.relpath(out_csv)}",
        ]
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        if summary_rows:
            labels = [row["method"] for row in summary_rows]
            data = [sanitize(grouped[label]) for label in labels]
            fig, ax = plt.subplots(figsize=(max(10, len(labels) * 0.5), 6))
            ax.boxplot(data, vert=True, showfliers=False)
            ax.set_title(f"{args.metric} distribution (seconds)")
            ax.set_ylabel("seconds")
            ax.set_xticks(np.arange(1, len(labels) + 1))
            ax.set_xticklabels(labels, rotation=45, ha="right", fontsize=7)
            ax.grid(axis="y", linestyle=":", alpha=0.4)
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

    print("Wrote recovery distribution:", out_pdf)
    print("Wrote summary CSV:", out_csv)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
