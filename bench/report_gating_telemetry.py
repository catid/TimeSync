#!/usr/bin/env python3
import argparse
import csv
import statistics
from collections import defaultdict


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
    ap = argparse.ArgumentParser(description="Summarize gating telemetry from peer_bench.csv.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="gating_telemetry.md", help="Output markdown path")
    ap.add_argument("--scenario", default="", help="Optional scenario filter")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    grouped = defaultdict(list)
    for r in rows:
        if args.scenario and r.get("scenario") != args.scenario:
            continue
        key = (r["scenario"], r["method"], r["estimator"], r["discipline"])
        grouped[key].append(r)

    lines = []
    lines.append("# Gating Telemetry Summary")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    if args.scenario:
        lines.append(f"Scenario: {args.scenario}")
    lines.append("")
    lines.append("| scenario | method | estimator | discipline | short_p10_ab | short_p10_ba | "
                 "short_iqr_ab | short_iqr_ba | near_hits_ab | near_hits_ba | "
                 "gsp_prom_ab | gsp_prom_ba | step_guard_ticks_ab | step_guard_ticks_ba | "
                 "step_guard_all_ok_ab | step_guard_all_ok_ba |")
    lines.append("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |")

    for key, items in sorted(grouped.items()):
        scenario, method, estimator, discipline = key
        def med(key):
            return median([to_float(r, key) for r in items])
        lines.append("| " + " | ".join([
            scenario,
            method,
            estimator,
            discipline,
            f"{med('short_p10_mean_ab_us'):.0f}",
            f"{med('short_p10_mean_ba_us'):.0f}",
            f"{med('short_iqr_mean_ab_us'):.0f}",
            f"{med('short_iqr_mean_ba_us'):.0f}",
            f"{med('short_near_hits_mean_ab'):.0f}",
            f"{med('short_near_hits_mean_ba'):.0f}",
            f"{med('gsp_promotions_ab'):.0f}",
            f"{med('gsp_promotions_ba'):.0f}",
            f"{med('step_guard_ticks_ab'):.0f}",
            f"{med('step_guard_ticks_ba'):.0f}",
            f"{med('step_guard_all_ok_ab'):.0f}",
            f"{med('step_guard_all_ok_ba'):.0f}",
        ]) + " |")

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
