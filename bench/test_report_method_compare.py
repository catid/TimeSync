#!/usr/bin/env python3
import csv
import os
import tempfile
import subprocess

FIELDS = [
    "scenario",
    "method",
    "estimator",
    "discipline",
    "poll_time_err_valid_ab",
    "poll_time_err_valid_ba",
    "poll_time_err_p95_ab_us",
    "poll_time_err_p95_ba_us",
    "overhead_bps",
]


def write_rows(path, rows):
    with open(path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS)
        writer.writeheader()
        writer.writerows(rows)


def main():
    rows = [
        {
            "scenario": "E0_noise_floor",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "1000",
            "poll_time_err_p95_ba_us": "2000",
            "overhead_bps": "18",
        },
        {
            "scenario": "E0_noise_floor",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "18446700000000000000",
            "poll_time_err_p95_ba_us": "18446700000000000000",
            "overhead_bps": "18",
        },
        {
            "scenario": "E7_drift",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "3000",
            "poll_time_err_p95_ba_us": "4000",
            "overhead_bps": "18",
        },
        {
            "scenario": "E71_video_latency_ramp",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "9000",
            "poll_time_err_p95_ba_us": "8000",
            "overhead_bps": "18",
        },
        {
            "scenario": "E63_clock_step_1s_a_fwd",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "7000",
            "poll_time_err_p95_ba_us": "6000",
            "overhead_bps": "18",
        },
    ]
    with tempfile.TemporaryDirectory() as tmp:
        csv_path = os.path.join(tmp, "peer_bench.csv")
        out_path = os.path.join(tmp, "method_compare.md")
        write_rows(csv_path, rows)
        subprocess.run([
            os.path.join(os.path.dirname(__file__), "report_method_compare.py"),
            csv_path,
            "--baseline",
            "M4_TimeSync",
            "--all-scenarios",
            "--score",
            "--out",
            out_path,
        ], check=True, capture_output=True, text=True)
        with open(out_path, "r") as f:
            text = f.read()
        if "184467" in text:
            raise SystemExit("sentinel overflow leaked into report")
        if "| M4_TimeSync |" not in text:
            raise SystemExit("missing method row in report")
        if "2000" not in text:
            raise SystemExit("expected median error not present")
        if "Weighted Score Summary" not in text:
            raise SystemExit("missing score summary section")
        if "| M4_TimeSync | min | none |" in text and " | 9000 | 7000 |" not in text:
            raise SystemExit("expected video/steps group values not present")
        print("ok")


if __name__ == "__main__":
    main()
