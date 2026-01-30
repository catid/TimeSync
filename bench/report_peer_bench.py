#!/usr/bin/env python3
import csv
import os
import sys
from collections import defaultdict
from datetime import datetime

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


def bootstrap_ci(values, func=np.median, n=2000, alpha=0.05):
    if len(values) == 0:
        return (0.0, 0.0)
    vals = np.array(values)
    rng = np.random.default_rng(12345)
    stats = []
    for _ in range(n):
        sample = rng.choice(vals, size=len(vals), replace=True)
        stats.append(func(sample))
    stats = np.array(stats)
    lo = np.quantile(stats, alpha / 2.0)
    hi = np.quantile(stats, 1.0 - alpha / 2.0)
    return (float(lo), float(hi))


def paired_bootstrap_pvalue(a, b, n=2000):
    if len(a) == 0 or len(b) == 0:
        return 1.0
    a = np.array(a)
    b = np.array(b)
    if len(a) != len(b):
        m = min(len(a), len(b))
        a = a[:m]
        b = b[:m]
    diff = a - b
    rng = np.random.default_rng(4242)
    stats = []
    for _ in range(n):
        sample = rng.choice(diff, size=len(diff), replace=True)
        stats.append(np.mean(sample))
    stats = np.array(stats)
    p_lo = np.mean(stats <= 0.0)
    p_hi = np.mean(stats >= 0.0)
    return float(2.0 * min(p_lo, p_hi))


def holm_bonferroni(p_values):
    m = len(p_values)
    order = np.argsort(p_values)
    adjusted = [0.0] * m
    for rank, idx in enumerate(order):
        adjusted[idx] = min(1.0, (m - rank) * p_values[idx])
    return adjusted


def group_rows(rows):
    grouped = defaultdict(list)
    for row in rows:
        key = (row["scenario"], row["method"], row["estimator"], row["discipline"])
        grouped[key].append(row)
    return grouped


def metric_values(rows, key):
    return [to_float(r, key) for r in rows]


def main():
    if len(sys.argv) < 2:
        print("Usage: report_peer_bench.py <peer_bench.csv> [out_pdf]")
        return 1

    csv_path = sys.argv[1]
    out_pdf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(csv_path), "peer_bench_report.pdf")

    rows = load_rows(csv_path)
    if not rows:
        print("No rows in", csv_path)
        return 1

    grouped = group_rows(rows)

    scenarios = sorted({r["scenario"] for r in rows})
    methods = sorted({r["method"] for r in rows})

    with PdfPages(out_pdf) as pdf:
        # Cover
        fig = plt.figure(figsize=(8.27, 11.69))
        fig.suptitle("Peer Sync Benchmark Report", fontsize=18, y=0.98)
        ax = fig.add_axes([0.08, 0.08, 0.84, 0.84])
        ax.axis("off")
        lines = [
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            f"Input: {os.path.relpath(csv_path)}",
            f"Scenarios: {len(scenarios)}",
            f"Methods: {len(methods)}",
            f"Rows: {len(rows)}",
            "",
            "Metrics included: offset/OWD error, skew error, convergence time, overhead, teleop RMS/max.",
        ]
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=11)
        pdf.savefig(fig)
        plt.close(fig)

        # Scenario-level bar charts for offset/OWD
        for scenario in scenarios:
            fig, axes = plt.subplots(2, 1, figsize=(11, 7), sharex=True)
            labels = []
            offset_vals = []
            owd_vals = []
            for key, items in grouped.items():
                if key[0] != scenario:
                    continue
                label = f"{key[1]}|{key[2]}|{key[3]}"
                labels.append(label)
                offset_vals.append(np.median(metric_values(items, "offset_p95_ab_us")))
                owd_vals.append(np.median(metric_values(items, "owd_p95_ab_us")))
            if not labels:
                continue
            x = np.arange(len(labels))
            axes[0].bar(x, offset_vals, color="#4C78A8")
            axes[0].set_title(f"{scenario}: offset p95 (AB)")
            axes[0].set_ylabel("us")
            axes[1].bar(x, owd_vals, color="#F58518")
            axes[1].set_title(f"{scenario}: OWD p95 (AB)")
            axes[1].set_ylabel("us")
            axes[1].set_xticks(x)
            axes[1].set_xticklabels(labels, rotation=45, ha="right", fontsize=8)
            for ax in axes:
                ax.grid(axis="y", linestyle=":", alpha=0.4)
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

        # Pareto frontier: overhead vs OWD p95
        fig, ax = plt.subplots(figsize=(10, 6))
        for key, items in grouped.items():
            overhead = np.median(metric_values(items, "overhead_bps"))
            owd = np.median(metric_values(items, "owd_p95_ab_us"))
            ax.scatter(overhead, owd, label=f"{key[1]}|{key[2]}|{key[3]}")
        ax.set_xlabel("Overhead (bytes/sec)")
        ax.set_ylabel("OWD p95 (us)")
        ax.set_title("Overhead vs OWD p95 (median across seeds)")
        ax.grid(linestyle=":", alpha=0.4)
        ax.legend(fontsize=6, ncol=2)
        fig.tight_layout()
        pdf.savefig(fig)
        plt.close(fig)

        # Teleop metrics
        teleop_rows = [r for r in rows if r["scenario"].startswith("E9")]
        if teleop_rows:
            fig, ax = plt.subplots(figsize=(10, 6))
            teleop_groups = defaultdict(list)
            for r in teleop_rows:
                key = f"{r['method']}|{r['estimator']}|{r['discipline']}"
                teleop_groups[key].append(to_float(r, "teleop_rms_error"))
            labels = list(teleop_groups.keys())
            vals = [np.median(teleop_groups[k]) for k in labels]
            ax.bar(range(len(labels)), vals, color="#54A24B")
            ax.set_title("Teleop RMS tracking error (median)")
            ax.set_ylabel("RMS error")
            ax.set_xticks(range(len(labels)))
            ax.set_xticklabels(labels, rotation=45, ha="right", fontsize=8)
            ax.grid(axis="y", linestyle=":", alpha=0.4)
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

        # CI table for offset/OWD
        fig = plt.figure(figsize=(11, 8.5))
        ax = fig.add_axes([0.05, 0.05, 0.9, 0.9])
        ax.axis("off")
        lines = ["Bootstrap 95% CI for offset/OWD p95 (median across seeds)", ""]
        for key, items in grouped.items():
            offset_vals = metric_values(items, "offset_p95_ab_us")
            owd_vals = metric_values(items, "owd_p95_ab_us")
            ci_offset = bootstrap_ci(offset_vals, func=np.median)
            ci_owd = bootstrap_ci(owd_vals, func=np.median)
            label = f"{key[0]} | {key[1]}|{key[2]}|{key[3]}"
            lines.append(f"{label}")
            lines.append(f"  offset p95 median CI: [{ci_offset[0]:.2f}, {ci_offset[1]:.2f}] us")
            lines.append(f"  owd   p95 median CI: [{ci_owd[0]:.2f}, {ci_owd[1]:.2f}] us")
        ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=9)
        pdf.savefig(fig)
        plt.close(fig)

        # Holm-Bonferroni comparisons vs TimeSync
        baseline = "M4_TimeSync"
        comparisons = []
        pvals = []
        for scenario in scenarios:
            base_key = None
            for key in grouped:
                if key[0] == scenario and key[1] == baseline:
                    base_key = key
                    break
            if base_key is None:
                continue
            base_rows = grouped[base_key]
            base_vals = metric_values(base_rows, "owd_p95_ab_us")
            for key, items in grouped.items():
                if key[0] != scenario or key[1] == baseline:
                    continue
                vals = metric_values(items, "owd_p95_ab_us")
                p = paired_bootstrap_pvalue(vals, base_vals)
                comparisons.append((scenario, key))
                pvals.append(p)
        if pvals:
            adj = holm_bonferroni(pvals)
            fig = plt.figure(figsize=(11, 8.5))
            ax = fig.add_axes([0.05, 0.05, 0.9, 0.9])
            ax.axis("off")
            lines = ["Holm-Bonferroni adjusted p-values (OWD p95 vs TimeSync)", ""]
            for i, (scenario, key) in enumerate(comparisons):
                label = f"{scenario} | {key[1]}|{key[2]}|{key[3]}"
                lines.append(f"{label}: p={pvals[i]:.4f}, adj={adj[i]:.4f}")
            ax.text(0.0, 1.0, "\n".join(lines), va="top", fontsize=9)
            pdf.savefig(fig)
            plt.close(fig)

    print(f"Wrote report to {out_pdf}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
