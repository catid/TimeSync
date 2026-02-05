#!/usr/bin/env python3
import csv
import sys
from collections import defaultdict
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


def main():
    if len(sys.argv) < 2:
        print("Usage: report_core_scenarios.py <peer_bench.csv> [out_md]")
        return 1

    csv_path = sys.argv[1]
    out_path = sys.argv[2] if len(sys.argv) > 2 else "core_scenarios_report.md"

    rows = load_rows(csv_path)
    if not rows:
        print("No rows in", csv_path)
        return 1

    scenarios = ["E0_noise_floor", "E1_stationary_jitter", "E7_drift"]
    grouped = defaultdict(list)
    for r in rows:
        if r.get("scenario") in scenarios:
            key = (r["scenario"], r["method"], r["estimator"], r["discipline"])
            grouped[key].append(r)

    lines = []
    lines.append("# Core Scenario Comparison (E0/E1/E7)")
    lines.append("")
    lines.append(f"Source: {csv_path}")
    lines.append("")

    for scenario in scenarios:
        lines.append(f"## {scenario}")
        lines.append("")
        lines.append("| method | estimator | discipline | poll_p95_ab_us | poll_p95_ba_us | overhead_bps | rows |")
        lines.append("| --- | --- | --- | --- | --- | --- | --- |")
        rows_out = []
        for key, items in grouped.items():
            if key[0] != scenario:
                continue
            p95_ab = median([to_float(r, "poll_time_err_p95_ab_us") for r in items])
            p95_ba = median([to_float(r, "poll_time_err_p95_ba_us") for r in items])
            overhead = median([to_float(r, "overhead_bps") for r in items])
            rows_out.append((p95_ab, p95_ba, overhead, key, len(items)))
        rows_out.sort(key=lambda x: max(x[0], x[1]))
        for p95_ab, p95_ba, overhead, key, count in rows_out:
            lines.append(
                f"| {key[1]} | {key[2]} | {key[3]} | "
                f"{p95_ab:.0f} | {p95_ba:.0f} | {overhead:.0f} | {count} |"
            )
        lines.append("")

    with open(out_path, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", out_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
