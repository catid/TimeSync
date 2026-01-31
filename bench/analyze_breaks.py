#!/usr/bin/env python3
import csv
import os
import sys

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


def main():
    if len(sys.argv) < 2:
        print("Usage: analyze_breaks.py <summary.csv> [out.txt]")
        return 1

    summary_path = sys.argv[1]
    out_path = sys.argv[2] if len(sys.argv) > 2 else None

    rows = list(csv.DictReader(open(summary_path)))
    if not rows:
        print("No rows in", summary_path)
        return 1

    breaks = []
    drift_rows = []

    for row in rows:
        name = row.get("name", "")
        time_p95 = max(time_metric(row, "p95_ab_us"), time_metric(row, "p95_ba_us"))
        time_p99 = max(time_metric(row, "p99_ab_us"), time_metric(row, "p99_ba_us"))
        owd_p95 = max(get_float(row, "owd_err_p95_ab_us"), get_float(row, "owd_err_p95_ba_us"))
        sync_time = max(get_float(row, "sync_time_a_s"), get_float(row, "sync_time_b_s"))
        metrics_start = get_float(row, "metrics_start_s")

        if "drift" in name:
            drift_rows.append((name, time_p95, time_p99, owd_p95, sync_time, metrics_start))

        broken = False
        reasons = []
        if sync_time == 0.0 or metrics_start == 0.0:
            broken = True
            reasons.append("not synchronized")
        if time_p95 > 5000.0:
            broken = True
            reasons.append("time_p95>5ms")
        if time_p99 > 10000.0:
            broken = True
            reasons.append("time_p99>10ms")

        if broken:
            breaks.append((name, ",".join(reasons), time_p95, time_p99, owd_p95, sync_time))

    lines = []
    lines.append(f"Total experiments: {len(rows)}")
    lines.append(f"Drift experiments: {len(drift_rows)}")
    lines.append("")

    if breaks:
        lines.append("Breaks (thresholds: time_p95>5ms, time_p99>10ms, or no sync):")
        for name, reasons, time_p95, time_p99, owd_p95, sync_time in breaks:
            lines.append(f"- {name}: {reasons} | time_p95={time_p95:.1f}us time_p99={time_p99:.1f}us owd_p95={owd_p95:.1f}us sync={sync_time:.2f}s")
    else:
        lines.append("No breaks found at thresholds.")

    lines.append("")
    if drift_rows:
        lines.append("Drift experiments (sorted by time_p95 desc):")
        drift_sorted = sorted(drift_rows, key=lambda x: x[1], reverse=True)
        for name, time_p95, time_p99, owd_p95, sync_time, metrics_start in drift_sorted:
            lines.append(f"- {name}: time_p95={time_p95:.1f}us time_p99={time_p99:.1f}us owd_p95={owd_p95:.1f}us sync={sync_time:.2f}s")

    output = "\n".join(lines)
    if out_path:
        with open(out_path, "w") as out_file:
            out_file.write(output)
    else:
        print(output)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
