#!/usr/bin/env python3
import argparse
import csv
from collections import defaultdict

def to_float(row, key):
    try:
        return float(row.get(key, 0.0))
    except ValueError:
        return 0.0


def main():
    ap = argparse.ArgumentParser(description="Summarize StepGuard gate diagnostics")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="stepguard_diag.md", help="Output markdown path")
    ap.add_argument("--method-prefix", default="M4_TimeSyncStepGuard", help="Method prefix filter")
    args = ap.parse_args()

    fields = {
        "ticks": ("step_guard_ticks_ab", "step_guard_ticks_ba"),
        "stable": ("step_guard_stable_ab", "step_guard_stable_ba"),
        "enough": ("step_guard_enough_ab", "step_guard_enough_ba"),
        "nonear": ("step_guard_nonear_ab", "step_guard_nonear_ba"),
        "near_ratio": ("step_guard_near_ratio_ok_ab", "step_guard_near_ratio_ok_ba"),
        "rtt_ok": ("step_guard_rtt_ok_ab", "step_guard_rtt_ok_ba"),
        "xor_ok": ("step_guard_xor_ok_ab", "step_guard_xor_ok_ba"),
        "sym_ok": ("step_guard_sym_ok_ab", "step_guard_sym_ok_ba"),
        "stale_ab": ("step_guard_stale_ab_ab", "step_guard_stale_ab_ba"),
        "stale_ba": ("step_guard_stale_ba_ab", "step_guard_stale_ba_ba"),
        "innov_ok": ("step_guard_innov_ok_ab", "step_guard_innov_ok_ba"),
        "all_ok": ("step_guard_all_ok_ab", "step_guard_all_ok_ba"),
    }

    totals = defaultdict(lambda: defaultdict(float))
    with open(args.csv_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            method = row.get("method", "")
            if not method.startswith(args.method_prefix):
                continue
            scenario = row.get("scenario", "")
            key = (scenario, method)
            for name, (ab, ba) in fields.items():
                totals[key][name] += to_float(row, ab) + to_float(row, ba)

    lines = []
    lines.append("# StepGuard Gate Diagnostics\n")
    lines.append(f"Source: {args.csv_path}\n")
    lines.append("\n")
    lines.append("Ratios are gate_hits / ticks (aggregated across seeds).\n")
    lines.append("\n")

    scenarios = sorted({k[0] for k in totals.keys()})
    methods = sorted({k[1] for k in totals.keys()})

    for sc in scenarios:
        lines.append(f"## {sc}\n")
        lines.append("| method | ticks | stable | enough | no_near | near_ratio | rtt_ok | xor_ok | sym_ok | innov_ok | all_ok | stale_ab | stale_ba |\n")
        lines.append("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |\n")
        for method in methods:
            key = (sc, method)
            if key not in totals:
                continue
            tick = totals[key]["ticks"]
            if tick <= 0:
                continue
            def ratio(name):
                return totals[key][name] / tick if tick > 0 else 0.0
            lines.append(
                "| {} | {:.0f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} | {:.3f} |\n".format(
                    method,
                    tick,
                    ratio("stable"),
                    ratio("enough"),
                    ratio("nonear"),
                    ratio("near_ratio"),
                    ratio("rtt_ok"),
                    ratio("xor_ok"),
                    ratio("sym_ok"),
                    ratio("innov_ok"),
                    ratio("all_ok"),
                    ratio("stale_ab"),
                    ratio("stale_ba"),
                )
            )
        lines.append("\n")

    with open(args.out, "w", newline="") as f:
        f.writelines(lines)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
