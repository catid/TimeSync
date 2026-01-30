#!/usr/bin/env python3
import csv
import sys

GOOD_P95 = 5000.0
GOOD_P99 = 10000.0
OK_P95 = 10000.0
OK_P99 = 20000.0


def get_float(row, key, default=0.0):
    try:
        return float(row.get(key, ""))
    except ValueError:
        return default


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


def main():
    if len(sys.argv) < 3:
        print("Usage: choose_drift_ranges.py <csv> <axis>")
        return 1

    path = sys.argv[1]
    axis = sys.argv[2]

    rows = list(csv.DictReader(open(path)))
    if not rows:
        return 1

    drift_vals = []
    axis_vals = []
    boundary = []

    for row in rows:
        drift = abs(get_float(row, "drift_a_ppm"))
        ax = axis_value(row, axis)
        time_p95 = max(get_float(row, "time_err_p95_ab_us"), get_float(row, "time_err_p95_ba_us"))
        time_p99 = max(get_float(row, "time_err_p99_ab_us"), get_float(row, "time_err_p99_ba_us"))

        drift_vals.append(drift)
        axis_vals.append(ax)

        # boundary: transition between good and acceptable
        if (GOOD_P95 < time_p95 <= OK_P95) or (GOOD_P99 < time_p99 <= OK_P99):
            boundary.append((drift, ax))

    if boundary:
        d_vals = [b[0] for b in boundary]
        a_vals = [b[1] for b in boundary]
        drift_min = min(d_vals)
        drift_max = max(d_vals)
        axis_min = min(a_vals)
        axis_max = max(a_vals)
    else:
        drift_min = min(drift_vals)
        drift_max = max(drift_vals)
        axis_min = min(axis_vals)
        axis_max = max(axis_vals)

    # Expand by 10% margin
    def expand(minv, maxv):
        if maxv <= minv:
            return minv, maxv
        margin = 0.1 * (maxv - minv)
        return max(minv - margin, 0.0), maxv + margin

    drift_min, drift_max = expand(drift_min, drift_max)
    axis_min, axis_max = expand(axis_min, axis_max)

    print(f"drift_min={drift_min}")
    print(f"drift_max={drift_max}")
    print(f"axis_min={axis_min}")
    print(f"axis_max={axis_max}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
