#!/usr/bin/env python3
import argparse
import csv
import sys

DEFAULT_MINDELTA_US = 1_000_000

def main():
    ap = argparse.ArgumentParser(description="Check apples-to-apples fairness for TimeSync methods")
    ap.add_argument("csv", help="peer_bench.csv output")
    ap.add_argument("--mindelta-us", type=int, default=DEFAULT_MINDELTA_US,
                    help="Required mindelta interval in microseconds for TimeSync methods")
    ap.add_argument("--method-prefix", default="M4_TimeSync",
                    help="Method prefix to enforce mindelta interval on")
    ap.add_argument("--allow-probe-methods", default="",
                    help="Comma-separated list of methods allowed to use probes (exact match)")
    ap.add_argument("--max-probe-rate", type=float, default=0.0,
                    help="If >0, flag any probe_rate_hz above this value")
    args = ap.parse_args()

    allow_probe = set(m for m in args.allow_probe_methods.split(",") if m)
    bad = []
    bad_probe = []
    bad_probe_rate = []
    with open(args.csv, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            method = row.get("method", "")
            mindelta = row.get("mindelta_interval_us")
            if not method.startswith(args.method_prefix):
                continue
            if mindelta is None:
                continue
            try:
                mindelta_val = int(float(mindelta))
            except ValueError:
                continue
            if mindelta_val != args.mindelta_us:
                bad.append((row.get("scenario"), method, mindelta_val))
            probe_rate = row.get("probe_rate_hz")
            if probe_rate is not None:
                try:
                    probe_val = float(probe_rate)
                except ValueError:
                    probe_val = 0.0
                if probe_val > 0.0 and method not in allow_probe:
                    bad_probe.append((row.get("scenario"), method, probe_val))
                if args.max_probe_rate > 0.0 and probe_val > args.max_probe_rate:
                    bad_probe_rate.append((row.get("scenario"), method, probe_val))

    if bad:
        print("Non-uniform mindelta interval detected:")
        for scenario, method, mindelta_val in bad:
            print(f"  scenario={scenario} method={method} mindelta_interval_us={mindelta_val}")
        return 1
    if bad_probe:
        print("Unexpected probe usage detected:")
        for scenario, method, probe_val in bad_probe:
            print(f"  scenario={scenario} method={method} probe_rate_hz={probe_val}")
        return 1
    if bad_probe_rate:
        print("Probe rate exceeds max:")
        for scenario, method, probe_val in bad_probe_rate:
            print(f"  scenario={scenario} method={method} probe_rate_hz={probe_val}")
        return 1

    print(f"All {args.method_prefix} rows use mindelta_interval_us={args.mindelta_us}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
