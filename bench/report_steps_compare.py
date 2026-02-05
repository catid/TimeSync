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


MAX_ERR_US = 1.0e15
MAX_RECOVER_S = 1.0e6


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def sanitize_err_us(value):
    if value <= 0.0:
        return None
    if value > MAX_ERR_US:
        return None
    return value


def sanitize_recover_s(value):
    if value <= 0.0:
        return None
    if value > MAX_RECOVER_S:
        return None
    return value


def recover_seconds(row, key_s, key_us):
    if key_us in row and row.get(key_us) not in (None, ""):
        val_us = to_float(row, key_us, 0.0)
        return val_us / 1_000_000.0
    return to_float(row, key_s, 0.0)


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def filtered_rows(rows, scenarios, method_filter, require_poll):
    for r in rows:
        sc = r.get("scenario", "")
        if sc not in scenarios:
            continue
        if require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        if method_filter and key[0] not in method_filter:
            continue
        yield r


def build_rows_out(items, scenarios):
    grouped = defaultdict(list)
    for r in items:
        key = (r["method"], r["estimator"], r["discipline"])
        grouped[key].append(r)

    rows_out = []
    for key, group_items in grouped.items():
        scenario_vals = {}
        recover_vals = {}
        recover3_vals = {}
        recover5_vals = {}
        recover3_5_vals = {}
        recover10_vals = {}
        recover3_10_vals = {}
        recover20_vals = {}
        recover3_20_vals = {}
        recover50_vals = {}
        recover3_50_vals = {}
        recover100_vals = {}
        recover3_100_vals = {}
        recover500_vals = {}
        recover3_500_vals = {}
        for sc in scenarios:
            vals = [max(to_float(r, "poll_time_err_p95_ab_us"),
                        to_float(r, "poll_time_err_p95_ba_us"))
                    for r in group_items if r["scenario"] == sc]
            cleaned = [sanitize_err_us(v) for v in vals]
            scenario_vals[sc] = median([v for v in cleaned if v is not None])
            rec = [max(recover_seconds(r, "step_recover_ab_s", "step_recover_ab_us"),
                       recover_seconds(r, "step_recover_ba_s", "step_recover_ba_us"))
                   for r in group_items if r["scenario"] == sc]
            rec_clean = [sanitize_recover_s(v) for v in rec]
            recover_vals[sc] = median([v for v in rec_clean if v is not None])
            rec3 = [max(recover_seconds(r, "step_recover3_ab_s", "step_recover3_ab_us"),
                        recover_seconds(r, "step_recover3_ba_s", "step_recover3_ba_us"))
                    for r in group_items if r["scenario"] == sc]
            rec3_clean = [sanitize_recover_s(v) for v in rec3]
            recover3_vals[sc] = median([v for v in rec3_clean if v is not None])
            rec5 = [max(recover_seconds(r, "step_recover5_ab_s", "step_recover5_ab_us"),
                        recover_seconds(r, "step_recover5_ba_s", "step_recover5_ba_us"))
                    for r in group_items if r["scenario"] == sc]
            rec5_clean = [sanitize_recover_s(v) for v in rec5]
            recover5_vals[sc] = median([v for v in rec5_clean if v is not None])
            rec3_5 = [max(recover_seconds(r, "step_recover3_5_ab_s", "step_recover3_5_ab_us"),
                          recover_seconds(r, "step_recover3_5_ba_s", "step_recover3_5_ba_us"))
                      for r in group_items if r["scenario"] == sc]
            rec3_5_clean = [sanitize_recover_s(v) for v in rec3_5]
            recover3_5_vals[sc] = median([v for v in rec3_5_clean if v is not None])
            rec10 = [max(recover_seconds(r, "step_recover10_ab_s", "step_recover10_ab_us"),
                         recover_seconds(r, "step_recover10_ba_s", "step_recover10_ba_us"))
                     for r in group_items if r["scenario"] == sc]
            rec10_clean = [sanitize_recover_s(v) for v in rec10]
            recover10_vals[sc] = median([v for v in rec10_clean if v is not None])
            rec3_10 = [max(recover_seconds(r, "step_recover3_10_ab_s", "step_recover3_10_ab_us"),
                           recover_seconds(r, "step_recover3_10_ba_s", "step_recover3_10_ba_us"))
                       for r in group_items if r["scenario"] == sc]
            rec3_10_clean = [sanitize_recover_s(v) for v in rec3_10]
            recover3_10_vals[sc] = median([v for v in rec3_10_clean if v is not None])
            rec20 = [max(recover_seconds(r, "step_recover20_ab_s", "step_recover20_ab_us"),
                         recover_seconds(r, "step_recover20_ba_s", "step_recover20_ba_us"))
                     for r in group_items if r["scenario"] == sc]
            rec20_clean = [sanitize_recover_s(v) for v in rec20]
            recover20_vals[sc] = median([v for v in rec20_clean if v is not None])
            rec3_20 = [max(recover_seconds(r, "step_recover3_20_ab_s", "step_recover3_20_ab_us"),
                           recover_seconds(r, "step_recover3_20_ba_s", "step_recover3_20_ba_us"))
                       for r in group_items if r["scenario"] == sc]
            rec3_20_clean = [sanitize_recover_s(v) for v in rec3_20]
            recover3_20_vals[sc] = median([v for v in rec3_20_clean if v is not None])
            rec50 = [max(recover_seconds(r, "step_recover50_ab_s", "step_recover50_ab_us"),
                         recover_seconds(r, "step_recover50_ba_s", "step_recover50_ba_us"))
                     for r in group_items if r["scenario"] == sc]
            rec50_clean = [sanitize_recover_s(v) for v in rec50]
            recover50_vals[sc] = median([v for v in rec50_clean if v is not None])
            rec3_50 = [max(recover_seconds(r, "step_recover3_50_ab_s", "step_recover3_50_ab_us"),
                           recover_seconds(r, "step_recover3_50_ba_s", "step_recover3_50_ba_us"))
                       for r in group_items if r["scenario"] == sc]
            rec3_50_clean = [sanitize_recover_s(v) for v in rec3_50]
            recover3_50_vals[sc] = median([v for v in rec3_50_clean if v is not None])
            rec100 = [max(recover_seconds(r, "step_recover100_ab_s", "step_recover100_ab_us"),
                          recover_seconds(r, "step_recover100_ba_s", "step_recover100_ba_us"))
                      for r in group_items if r["scenario"] == sc]
            rec100_clean = [sanitize_recover_s(v) for v in rec100]
            recover100_vals[sc] = median([v for v in rec100_clean if v is not None])
            rec3_100 = [max(recover_seconds(r, "step_recover3_100_ab_s", "step_recover3_100_ab_us"),
                            recover_seconds(r, "step_recover3_100_ba_s", "step_recover3_100_ba_us"))
                        for r in group_items if r["scenario"] == sc]
            rec3_100_clean = [sanitize_recover_s(v) for v in rec3_100]
            recover3_100_vals[sc] = median([v for v in rec3_100_clean if v is not None])
            rec500 = [max(recover_seconds(r, "step_recover500_ab_s", "step_recover500_ab_us"),
                          recover_seconds(r, "step_recover500_ba_s", "step_recover500_ba_us"))
                      for r in group_items if r["scenario"] == sc]
            rec500_clean = [sanitize_recover_s(v) for v in rec500]
            recover500_vals[sc] = median([v for v in rec500_clean if v is not None])
            rec3_500 = [max(recover_seconds(r, "step_recover3_500_ab_s", "step_recover3_500_ab_us"),
                            recover_seconds(r, "step_recover3_500_ba_s", "step_recover3_500_ba_us"))
                        for r in group_items if r["scenario"] == sc]
            rec3_500_clean = [sanitize_recover_s(v) for v in rec3_500]
            recover3_500_vals[sc] = median([v for v in rec3_500_clean if v is not None])
        rec_list = [v for v in recover_vals.values() if v > 0.0]
        rec_med = median(rec_list) if rec_list else 0.0
        rows_out.append((key, scenario_vals, recover_vals, recover3_vals,
                         recover5_vals, recover3_5_vals, recover10_vals, recover3_10_vals,
                         recover20_vals, recover3_20_vals, recover50_vals, recover3_50_vals,
                         recover100_vals, recover3_100_vals, recover500_vals, recover3_500_vals, rec_med))

    rows_out.sort(key=lambda x: x[16] if x[16] > 0 else 1e9)
    return rows_out


def format_section(title, rows_out, scenarios):
    lines = []
    lines.append(f"## {title}")
    lines.append("")
    header = "| method | estimator | discipline | recovery_med_s | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (4 + len(scenarios))) + " |")
    for (method, est, disc), sc_vals, _, _, _, _, _, _, _, _, _, _, _, _, _, _, rec_med in rows_out:
        row = f"| {method} | {est} | {disc} | {rec_med:.2f} | "
        row += " | ".join(f"{sc_vals.get(sc, 0.0):.0f}" for sc in scenarios)
        row += " |"
        lines.append(row)

    lines.append("")
    lines.append("### Step Recovery Times (seconds, median across seeds)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, rec_vals, _, _, _, _, _, _, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (3 consecutive polls under threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, rec3_vals, _, _, _, _, _, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (5 ms threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, rec5_vals, _, _, _, _, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec5_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (5 ms threshold, 3 consecutive polls)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, rec3_5_vals, _, _, _, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_5_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (10 ms threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, rec10_vals, _, _, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec10_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (10 ms threshold, 3 consecutive polls)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, rec3_10_vals, _, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_10_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (20 ms threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, rec20_vals, _, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec20_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (20 ms threshold, 3 consecutive polls)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, rec3_20_vals, _, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_20_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (50 ms threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, _, rec50_vals, _, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec50_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (50 ms threshold, 3 consecutive polls)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, _, _, rec3_50_vals, _, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_50_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (100 ms threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, _, _, _, rec100_vals, _, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec100_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (100 ms threshold, 3 consecutive polls)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, _, _, _, _, rec3_100_vals, _, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_100_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (500 ms threshold)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, _, _, _, _, _, rec500_vals, _, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec500_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    lines.append("")

    lines.append("### Step Recovery Times (500 ms threshold, 3 consecutive polls)")
    lines.append("")
    header = "| method | estimator | discipline | " + " | ".join(scenarios) + " |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * (3 + len(scenarios))) + " |")
    for (method, est, disc), _, _, _, _, _, _, _, _, _, _, _, _, _, _, rec3_500_vals, _ in rows_out:
        row = f"| {method} | {est} | {disc} | "
        row += " | ".join(f"{rec3_500_vals.get(sc, 0.0):.2f}" for sc in scenarios)
        row += " |"
        lines.append(row)
    return lines


def main():
    ap = argparse.ArgumentParser(description="Compare step scenarios (E14–E21) with recovery metrics.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="steps_compare.md", help="Output markdown path")
    ap.add_argument("--methods", default="", help="Comma-separated method labels to include (exact match)")
    ap.add_argument("--require-poll", action="store_true",
                    help="Ignore rows with poll_time_err_valid_ab/ba == 0")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    scenarios = sorted({r["scenario"] for r in rows if r.get("scenario", "").startswith(
        ("E14", "E15", "E16", "E17", "E18", "E19", "E20", "E21",
         "E45", "E53",
         "E63", "E64", "E65", "E66", "E67", "E68", "E69", "E70"))})

    method_filter = [m.strip() for m in args.methods.split(",") if m.strip()] if args.methods else []

    filtered = list(filtered_rows(rows, scenarios, method_filter, args.require_poll))
    if not filtered:
        print("No matching rows after filtering.")
        return 1

    poll_rates = sorted({to_float(r, "poll_rate_hz", -1.0) for r in filtered})

    lines = []
    lines.append("# Step Scenario Compare (E14–E21)")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    lines.append("")

    rows_out_all = build_rows_out(filtered, scenarios)
    lines.extend(format_section("All poll rates", rows_out_all, scenarios))

    for rate in poll_rates:
        label = "unknown" if rate < 0 else f"{rate:g} Hz"
        subset = [r for r in filtered if to_float(r, "poll_rate_hz", -1.0) == rate]
        if not subset:
            continue
        rows_out = build_rows_out(subset, scenarios)
        lines.extend(format_section(f"Poll rate {label}", rows_out, scenarios))

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
