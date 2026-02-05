#!/usr/bin/env python3
import csv
import os
import sys
from collections import defaultdict
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
OK_SYNC = 8.0


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


def classify_row(row):
    time_p95 = max(time_metric(row, "p95_ab_us"), time_metric(row, "p95_ba_us"))
    time_p99 = max(time_metric(row, "p99_ab_us"), time_metric(row, "p99_ba_us"))
    owd_p95 = max(to_float(row, "owd_p95_ab_us"), to_float(row, "owd_p95_ba_us"))
    sync = max(to_float(row, "sync_time_a_s"), to_float(row, "sync_time_b_s"))

    if time_p95 <= GOOD_P95 and time_p99 <= GOOD_P99 and owd_p95 <= GOOD_OWD and sync <= GOOD_SYNC:
        return 2
    if time_p95 <= OK_P95 and time_p99 <= OK_P99 and owd_p95 <= OK_OWD and sync <= OK_SYNC:
        return 1
    return 0


def scenario_class(name):
    lower = name.lower()
    if "video" in lower:
        return "video"
    if "step" in lower or "jump" in lower or "freeze" in lower:
        return "step"
    if "drift" in lower or "skew" in lower or "sine" in lower:
        return "drift"
    if "loss" in lower or "outage" in lower or "drop" in lower:
        return "loss"
    if "reorder" in lower or "dup" in lower:
        return "reorder"
    return "noise"


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def label_for(row):
    return f"{row['method']}|{row['estimator']}|{row['discipline']}"


def main():
    if len(sys.argv) < 2:
        print("Usage: report_robustness_by_class.py <peer_bench.csv> [out_pdf] [out_png]")
        return 1

    csv_path = sys.argv[1]
    out_pdf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(csv_path), "robustness_by_class.pdf")
    out_png = sys.argv[3] if len(sys.argv) > 3 else ""

    rows = load_rows(csv_path)
    if not rows:
        print("No rows in", csv_path)
        return 1

    classes = ["noise", "loss", "reorder", "step", "drift", "video"]
    methods = sorted({label_for(row) for row in rows})

    grouped = defaultdict(list)
    for row in rows:
        grouped[(scenario_class(row["scenario"]), label_for(row))].append(row)

    scores = np.zeros((len(classes), len(methods)))
    scores[:] = np.nan
    for c_idx, cls in enumerate(classes):
        for m_idx, method in enumerate(methods):
            items = grouped.get((cls, method), [])
            if not items:
                continue
            values = [classify_row(r) for r in items]
            scores[c_idx, m_idx] = float(np.mean(values))

    with PdfPages(out_pdf) as pdf:
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("Robustness Heatmap by Scenario Class", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")
        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Input: {os.path.relpath(csv_path)}",
            f"Classes: {len(classes)}",
            f"Methods: {len(methods)}",
            "",
            "Score legend:",
            "2 = Good (<=5ms p95, <=10ms p99, <=5ms OWD p95, sync<=5s)",
            "1 = Acceptable (<=10ms p95, <=20ms p99, <=10ms OWD p95, sync<=8s)",
            "0 = Fail",
        ]
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        fig, ax = plt.subplots(figsize=(max(10, len(methods) * 0.5), 5))
        mesh = ax.imshow(scores, cmap="RdYlGn", vmin=0.0, vmax=2.0, aspect="auto")
        ax.set_title("Scenario Class x Method Robustness (avg score)")
        ax.set_xlabel("Method")
        ax.set_ylabel("Scenario Class")
        ax.set_xticks(np.arange(len(methods)))
        ax.set_xticklabels(methods, rotation=45, ha="right", fontsize=7)
        ax.set_yticks(np.arange(len(classes)))
        ax.set_yticklabels(classes, fontsize=9)
        cbar = fig.colorbar(mesh, ax=ax, fraction=0.046, pad=0.04)
        cbar.set_ticks([0.0, 1.0, 2.0])
        cbar.set_ticklabels(["Fail", "Acceptable", "Good"])
        fig.tight_layout()
        pdf.savefig(fig)
        if out_png:
            fig.savefig(out_png)
        plt.close(fig)

    print("Wrote robustness-by-class heatmap:", out_pdf)
    if out_png:
        print("Wrote robustness-by-class PNG:", out_png)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
