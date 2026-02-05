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


def classify(row):
    time_p95 = max(time_metric(row, "p95_ab_us"), time_metric(row, "p95_ba_us"))
    time_p99 = max(time_metric(row, "p99_ab_us"), time_metric(row, "p99_ba_us"))
    owd_p95 = max(to_float(row, "owd_p95_ab_us"), to_float(row, "owd_p95_ba_us"))
    sync = max(to_float(row, "sync_time_a_s"), to_float(row, "sync_time_b_s"))

    if time_p95 <= GOOD_P95 and time_p99 <= GOOD_P99 and owd_p95 <= GOOD_OWD and sync <= GOOD_SYNC:
        return "good"
    if time_p95 <= OK_P95 and time_p99 <= OK_P99 and owd_p95 <= OK_OWD and sync <= OK_SYNC:
        return "ok"
    return "fail"


def label_for(row):
    return f"{row['method']}|{row['estimator']}|{row['discipline']}"


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def main():
    if len(sys.argv) < 2:
        print("Usage: report_stability_hist.py <peer_bench.csv> [out_pdf]")
        return 1

    csv_path = sys.argv[1]
    out_pdf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(csv_path), "stability_histogram.pdf")

    rows = load_rows(csv_path)
    if not rows:
        print("No rows in", csv_path)
        return 1

    grouped = defaultdict(list)
    for row in rows:
        grouped[(row["scenario"], label_for(row))].append(row)

    method_counts = defaultdict(lambda: {"good": 0, "ok": 0, "fail": 0})
    for (scenario, method), items in grouped.items():
        if not items:
            continue
        # Aggregate per scenario+method by median metric
        def med(values):
            return float(np.median(values)) if values else 0.0
        agg = {
            "poll_time_err_p95_ab_us": med([to_float(r, "poll_time_err_p95_ab_us") for r in items]),
            "poll_time_err_p95_ba_us": med([to_float(r, "poll_time_err_p95_ba_us") for r in items]),
            "poll_time_err_p99_ab_us": med([to_float(r, "poll_time_err_p99_ab_us") for r in items]),
            "poll_time_err_p99_ba_us": med([to_float(r, "poll_time_err_p99_ba_us") for r in items]),
            "offset_p95_ab_us": med([to_float(r, "offset_p95_ab_us") for r in items]),
            "offset_p95_ba_us": med([to_float(r, "offset_p95_ba_us") for r in items]),
            "offset_p99_ab_us": med([to_float(r, "offset_p99_ab_us") for r in items]),
            "offset_p99_ba_us": med([to_float(r, "offset_p99_ba_us") for r in items]),
            "owd_p95_ab_us": med([to_float(r, "owd_p95_ab_us") for r in items]),
            "owd_p95_ba_us": med([to_float(r, "owd_p95_ba_us") for r in items]),
            "sync_time_a_s": med([to_float(r, "sync_time_a_s") for r in items]),
            "sync_time_b_s": med([to_float(r, "sync_time_b_s") for r in items]),
        }
        label = classify(agg)
        method_counts[method][label] += 1

    methods = sorted(method_counts.keys())
    out_csv = os.path.join(os.path.dirname(csv_path), "stability_histogram.csv")
    with open(out_csv, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["method", "good", "ok", "fail"])
        for m in methods:
            c = method_counts[m]
            writer.writerow([m, c["good"], c["ok"], c["fail"]])

    with PdfPages(out_pdf) as pdf:
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("Algorithm Stability Histogram", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")
        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Input: {os.path.relpath(csv_path)}",
            f"Methods: {len(methods)}",
            f"Output CSV: {os.path.relpath(out_csv)}",
            "",
            "Good/Ok/Fail classification based on p95/p99 time error, OWD p95, and sync time.",
        ]
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        if methods:
            good = np.array([method_counts[m]["good"] for m in methods])
            ok = np.array([method_counts[m]["ok"] for m in methods])
            fail = np.array([method_counts[m]["fail"] for m in methods])
            x = np.arange(len(methods))
            fig, ax = plt.subplots(figsize=(max(10, len(methods) * 0.5), 6))
            ax.bar(x, good, color="#54A24B", label="Good")
            ax.bar(x, ok, bottom=good, color="#F58518", label="Ok")
            ax.bar(x, fail, bottom=good + ok, color="#E45756", label="Fail")
            ax.set_title("Scenario stability per method")
            ax.set_ylabel("scenario count")
            ax.set_xticks(x)
            ax.set_xticklabels(methods, rotation=45, ha="right", fontsize=7)
            ax.legend()
            ax.grid(axis="y", linestyle=":", alpha=0.4)
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

    print("Wrote stability histogram:", out_pdf)
    print("Wrote summary CSV:", out_csv)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
