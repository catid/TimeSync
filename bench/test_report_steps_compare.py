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
    "poll_rate_hz",
    "poll_time_err_valid_ab",
    "poll_time_err_valid_ba",
    "poll_time_err_p95_ab_us",
    "poll_time_err_p95_ba_us",
    "step_recover_ab_s",
    "step_recover_ba_s",
    "step_recover_ab_us",
    "step_recover_ba_us",
    "step_recover3_ab_s",
    "step_recover3_ba_s",
    "step_recover3_ab_us",
    "step_recover3_ba_us",
    "step_recover5_ab_s",
    "step_recover5_ba_s",
    "step_recover5_ab_us",
    "step_recover5_ba_us",
    "step_recover3_5_ab_s",
    "step_recover3_5_ba_s",
    "step_recover3_5_ab_us",
    "step_recover3_5_ba_us",
    "step_recover10_ab_s",
    "step_recover10_ba_s",
    "step_recover10_ab_us",
    "step_recover10_ba_us",
    "step_recover3_10_ab_s",
    "step_recover3_10_ba_s",
    "step_recover3_10_ab_us",
    "step_recover3_10_ba_us",
    "step_recover20_ab_s",
    "step_recover20_ba_s",
    "step_recover20_ab_us",
    "step_recover20_ba_us",
    "step_recover3_20_ab_s",
    "step_recover3_20_ba_s",
    "step_recover3_20_ab_us",
    "step_recover3_20_ba_us",
    "step_recover50_ab_s",
    "step_recover50_ba_s",
    "step_recover50_ab_us",
    "step_recover50_ba_us",
    "step_recover3_50_ab_s",
    "step_recover3_50_ba_s",
    "step_recover3_50_ab_us",
    "step_recover3_50_ba_us",
    "step_recover100_ab_s",
    "step_recover100_ba_s",
    "step_recover100_ab_us",
    "step_recover100_ba_us",
    "step_recover3_100_ab_s",
    "step_recover3_100_ba_s",
    "step_recover3_100_ab_us",
    "step_recover3_100_ba_us",
    "step_recover500_ab_s",
    "step_recover500_ba_s",
    "step_recover500_ab_us",
    "step_recover500_ba_us",
    "step_recover3_500_ab_s",
    "step_recover3_500_ba_s",
    "step_recover3_500_ab_us",
    "step_recover3_500_ba_us",
]


def write_rows(path, rows):
    with open(path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS)
        writer.writeheader()
        writer.writerows(rows)


def main():
    rows = [
        {
            "scenario": "E14_clock_step_small",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_rate_hz": "1",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "1000",
            "poll_time_err_p95_ba_us": "2000",
            "step_recover_ab_s": "5",
            "step_recover_ba_s": "6",
            "step_recover_ab_us": "2000000",
            "step_recover_ba_us": "3000000",
            "step_recover3_ab_s": "7",
            "step_recover3_ba_s": "8",
            "step_recover3_ab_us": "4000000",
            "step_recover3_ba_us": "5000000",
            "step_recover5_ab_s": "9",
            "step_recover5_ba_s": "10",
            "step_recover5_ab_us": "6000000",
            "step_recover5_ba_us": "7000000",
            "step_recover3_5_ab_s": "11",
            "step_recover3_5_ba_s": "12",
            "step_recover3_5_ab_us": "8000000",
            "step_recover3_5_ba_us": "9000000",
            "step_recover10_ab_s": "13",
            "step_recover10_ba_s": "14",
            "step_recover10_ab_us": "10000000",
            "step_recover10_ba_us": "11000000",
            "step_recover3_10_ab_s": "15",
            "step_recover3_10_ba_s": "16",
            "step_recover3_10_ab_us": "12000000",
            "step_recover3_10_ba_us": "13000000",
            "step_recover20_ab_s": "17",
            "step_recover20_ba_s": "18",
            "step_recover20_ab_us": "14000000",
            "step_recover20_ba_us": "15000000",
            "step_recover3_20_ab_s": "19",
            "step_recover3_20_ba_s": "20",
            "step_recover3_20_ab_us": "16000000",
            "step_recover3_20_ba_us": "17000000",
            "step_recover50_ab_s": "21",
            "step_recover50_ba_s": "22",
            "step_recover50_ab_us": "18000000",
            "step_recover50_ba_us": "19000000",
            "step_recover3_50_ab_s": "23",
            "step_recover3_50_ba_s": "24",
            "step_recover3_50_ab_us": "20000000",
            "step_recover3_50_ba_us": "21000000",
            "step_recover100_ab_s": "25",
            "step_recover100_ba_s": "26",
            "step_recover100_ab_us": "22000000",
            "step_recover100_ba_us": "23000000",
            "step_recover3_100_ab_s": "27",
            "step_recover3_100_ba_s": "28",
            "step_recover3_100_ab_us": "24000000",
            "step_recover3_100_ba_us": "25000000",
            "step_recover500_ab_s": "29",
            "step_recover500_ba_s": "30",
            "step_recover500_ab_us": "26000000",
            "step_recover500_ba_us": "27000000",
            "step_recover3_500_ab_s": "31",
            "step_recover3_500_ba_s": "32",
            "step_recover3_500_ab_us": "28000000",
            "step_recover3_500_ba_us": "29000000",
        },
        {
            "scenario": "E14_clock_step_small",
            "method": "M4_TimeSync",
            "estimator": "min",
            "discipline": "none",
            "poll_rate_hz": "5",
            "poll_time_err_valid_ab": "1",
            "poll_time_err_valid_ba": "1",
            "poll_time_err_p95_ab_us": "18446700000000000000",
            "poll_time_err_p95_ba_us": "18446700000000000000",
            "step_recover_ab_s": "-1",
            "step_recover_ba_s": "-1",
            "step_recover_ab_us": "-1",
            "step_recover_ba_us": "-1",
            "step_recover3_ab_s": "-1",
            "step_recover3_ba_s": "-1",
            "step_recover3_ab_us": "-1",
            "step_recover3_ba_us": "-1",
            "step_recover5_ab_s": "-1",
            "step_recover5_ba_s": "-1",
            "step_recover5_ab_us": "-1",
            "step_recover5_ba_us": "-1",
            "step_recover3_5_ab_s": "-1",
            "step_recover3_5_ba_s": "-1",
            "step_recover3_5_ab_us": "-1",
            "step_recover3_5_ba_us": "-1",
            "step_recover10_ab_s": "-1",
            "step_recover10_ba_s": "-1",
            "step_recover10_ab_us": "-1",
            "step_recover10_ba_us": "-1",
            "step_recover3_10_ab_s": "-1",
            "step_recover3_10_ba_s": "-1",
            "step_recover3_10_ab_us": "-1",
            "step_recover3_10_ba_us": "-1",
            "step_recover20_ab_s": "-1",
            "step_recover20_ba_s": "-1",
            "step_recover20_ab_us": "-1",
            "step_recover20_ba_us": "-1",
            "step_recover3_20_ab_s": "-1",
            "step_recover3_20_ba_s": "-1",
            "step_recover3_20_ab_us": "-1",
            "step_recover3_20_ba_us": "-1",
            "step_recover50_ab_s": "-1",
            "step_recover50_ba_s": "-1",
            "step_recover50_ab_us": "-1",
            "step_recover50_ba_us": "-1",
            "step_recover3_50_ab_s": "-1",
            "step_recover3_50_ba_s": "-1",
            "step_recover3_50_ab_us": "-1",
            "step_recover3_50_ba_us": "-1",
            "step_recover100_ab_s": "-1",
            "step_recover100_ba_s": "-1",
            "step_recover100_ab_us": "-1",
            "step_recover100_ba_us": "-1",
            "step_recover3_100_ab_s": "-1",
            "step_recover3_100_ba_s": "-1",
            "step_recover3_100_ab_us": "-1",
            "step_recover3_100_ba_us": "-1",
            "step_recover500_ab_s": "-1",
            "step_recover500_ba_s": "-1",
            "step_recover500_ab_us": "-1",
            "step_recover500_ba_us": "-1",
            "step_recover3_500_ab_s": "-1",
            "step_recover3_500_ba_s": "-1",
            "step_recover3_500_ab_us": "-1",
            "step_recover3_500_ba_us": "-1",
        },
    ]
    with tempfile.TemporaryDirectory() as tmp:
        csv_path = os.path.join(tmp, "peer_bench.csv")
        out_path = os.path.join(tmp, "steps_compare.md")
        write_rows(csv_path, rows)
        result = subprocess.run([
            os.path.join(os.path.dirname(__file__), "report_steps_compare.py"),
            csv_path,
            "--out",
            out_path,
            "--require-poll",
        ], check=True, capture_output=True, text=True)
        with open(out_path, "r") as f:
            text = f.read()
        if "184467" in text:
            raise SystemExit("sentinel overflow leaked into report")
        if "| M4_TimeSync |" not in text:
            raise SystemExit("missing method row in report")
        if "| M4_TimeSync |" in text and "2000" not in text:
            raise SystemExit("expected median error not present")
        if "3.00" not in text:
            raise SystemExit("expected recovery seconds derived from us not present")
        if "7.00" not in text:
            raise SystemExit("expected 5ms recovery seconds derived from us not present")
        if "11.00" not in text:
            raise SystemExit("expected 10ms recovery seconds derived from us not present")
        if "15.00" not in text:
            raise SystemExit("expected 20ms recovery seconds derived from us not present")
        if "19.00" not in text:
            raise SystemExit("expected 50ms recovery seconds derived from us not present")
        if "25.00" not in text:
            raise SystemExit("expected 100ms recovery seconds derived from us not present")
        if "29.00" not in text:
            raise SystemExit("expected 500ms recovery seconds derived from us not present")
        if "Poll rate 1 Hz" not in text:
            raise SystemExit("missing per-poll-rate section")
        if "3 consecutive polls" not in text:
            raise SystemExit("missing consecutive recovery section")
        if "5 ms threshold" not in text:
            raise SystemExit("missing 5 ms recovery section")
        if "10 ms threshold" not in text:
            raise SystemExit("missing 10 ms recovery section")
        if "20 ms threshold" not in text:
            raise SystemExit("missing 20 ms recovery section")
        if "50 ms threshold" not in text:
            raise SystemExit("missing 50 ms recovery section")
        if "100 ms threshold" not in text:
            raise SystemExit("missing 100 ms recovery section")
        if "500 ms threshold" not in text:
            raise SystemExit("missing 500 ms recovery section")
        print("ok")


if __name__ == "__main__":
    main()
