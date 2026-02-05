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
    ap = argparse.ArgumentParser(description="Summarize DD-AC step diagnostics (E14–E21).")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="ddac_step_diag.md", help="Output markdown path")
    ap.add_argument("--methods", default="", help="Comma-separated method labels to include (exact match)")
    ap.add_argument("--require-poll", action="store_true",
                    help="Ignore rows with poll_time_err_valid_ab/ba == 0")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    scenarios = sorted({r["scenario"] for r in rows if r["scenario"].startswith(
        ("E14", "E15", "E16", "E17", "E18", "E19", "E20", "E21"))})

    method_filter = [m.strip() for m in args.methods.split(",") if m.strip()] if args.methods else []

    grouped = defaultdict(list)
    for r in rows:
        sc = r.get("scenario", "")
        if sc not in scenarios:
            continue
        if args.require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        if method_filter and key[0] not in method_filter:
            continue
        grouped[key].append(r)

    rows_out = []
    for key, items in grouped.items():
        gap_in_mean = []
        gap_in_p95 = []
        gap_out_mean = []
        gap_out_p95 = []
        step_resets = []
        step_in_hits = []
        step_out_hits = []
        step_xor_hits = []
        step_streak_max = []

        for r in items:
            gap_in_mean.append(max(
                to_float(r, "ddac_gap_in_mean_ab_us"),
                to_float(r, "ddac_gap_in_mean_ba_us"),
            ))
            gap_in_p95.append(max(
                to_float(r, "ddac_gap_in_p95_ab_us"),
                to_float(r, "ddac_gap_in_p95_ba_us"),
            ))
            gap_out_mean.append(max(
                to_float(r, "ddac_gap_out_mean_ab_us"),
                to_float(r, "ddac_gap_out_mean_ba_us"),
            ))
            gap_out_p95.append(max(
                to_float(r, "ddac_gap_out_p95_ab_us"),
                to_float(r, "ddac_gap_out_p95_ba_us"),
            ))
            step_resets.append(
                to_float(r, "ddac_step_resets_ab") + to_float(r, "ddac_step_resets_ba")
            )
            step_in_hits.append(
                to_float(r, "ddac_step_in_hits_ab") + to_float(r, "ddac_step_in_hits_ba")
            )
            step_out_hits.append(
                to_float(r, "ddac_step_out_hits_ab") + to_float(r, "ddac_step_out_hits_ba")
            )
            step_xor_hits.append(
                to_float(r, "ddac_step_xor_hits_ab") + to_float(r, "ddac_step_xor_hits_ba")
            )
            step_streak_max.append(max(
                to_float(r, "ddac_step_streak_max_ab"),
                to_float(r, "ddac_step_streak_max_ba"),
            ))

        rows_out.append((
            key,
            median(gap_in_mean),
            median(gap_in_p95),
            median(gap_out_mean),
            median(gap_out_p95),
            median(step_resets),
            median(step_in_hits),
            median(step_out_hits),
            median(step_xor_hits),
            median(step_streak_max),
        ))

    rows_out.sort(key=lambda x: x[5], reverse=True)

    lines = []
    lines.append("# DD-AC Step Diagnostics (E14–E21)")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    lines.append("")
    header = ("| method | estimator | discipline | gap_in_mean_us | gap_in_p95_us | "
              "gap_out_mean_us | gap_out_p95_us | step_resets | step_in_hits | "
              "step_out_hits | step_xor_hits | step_streak_max |")
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * 12) + " |")
    for (method, est, disc), gi_m, gi_p95, go_m, go_p95, resets, in_hits, out_hits, xor_hits, streak_max in rows_out:
        lines.append(
            f"| {method} | {est} | {disc} | {gi_m:.1f} | {gi_p95:.1f} | "
            f"{go_m:.1f} | {go_p95:.1f} | {resets:.0f} | {in_hits:.0f} | "
            f"{out_hits:.0f} | {xor_hits:.0f} | {streak_max:.0f} |"
        )

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
