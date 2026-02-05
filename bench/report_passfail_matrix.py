#!/usr/bin/env python3
import argparse
import csv
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


def is_e0_e6(name):
    return name.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_"))


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def main():
    ap = argparse.ArgumentParser(description="Pass/fail matrix for E0–E6 scenarios.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--out", default="passfail_matrix.md", help="Output markdown path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method label")
    ap.add_argument("--tolerance", type=float, default=0.0,
                    help="Allowed % over baseline (E0–E6)")
    ap.add_argument("--require-poll", action="store_true",
                    help="Ignore rows with poll_time_err_valid_ab/ba == 0")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    scenarios = sorted({r["scenario"] for r in rows if is_e0_e6(r["scenario"])})
    if not scenarios:
        print("No E0–E6 scenarios found")
        return 1

    grouped = {}
    for r in rows:
        if not is_e0_e6(r["scenario"]):
            continue
        if args.require_poll:
            if int(to_float(r, "poll_time_err_valid_ab", 0)) == 0:
                continue
            if int(to_float(r, "poll_time_err_valid_ba", 0)) == 0:
                continue
        key = (r["method"], r["estimator"], r["discipline"])
        grouped.setdefault(key, []).append(r)

    baseline = {}
    for key, items in grouped.items():
        if key[0] != args.baseline:
            continue
        for sc in scenarios:
            vals = [
                max(to_float(r, "poll_time_err_p95_ab_us"),
                    to_float(r, "poll_time_err_p95_ba_us"))
                for r in items if r["scenario"] == sc
            ]
            baseline[sc] = median([v for v in vals if v > 0.0])
        break

    if not baseline:
        print("Baseline not found:", args.baseline)
        return 1

    lines = []
    lines.append("# E0–E6 pass/fail matrix")
    lines.append("")
    lines.append(f"Baseline: `{args.baseline}` with tolerance {args.tolerance:.1f}%")
    lines.append("")

    header = ["Method"] + scenarios + ["Pass/Total"]
    lines.append("| " + " | ".join(header) + " |")
    lines.append("| " + " | ".join(["---"] * len(header)) + " |")

    for key in sorted(grouped.keys()):
        method_label = key[0]
        items = grouped[key]
        passes = 0
        cells = []
        for sc in scenarios:
            vals = [
                max(to_float(r, "poll_time_err_p95_ab_us"),
                    to_float(r, "poll_time_err_p95_ba_us"))
                for r in items if r["scenario"] == sc
            ]
            value = median([v for v in vals if v > 0.0])
            base = baseline.get(sc, 0.0)
            if base <= 0.0 or value <= 0.0:
                cells.append("–")
                continue
            limit = base * (1.0 + args.tolerance / 100.0)
            ok = value <= limit
            cells.append("P" if ok else "F")
            if ok:
                passes += 1
        cells.append(f"{passes}/{len(scenarios)}")
        lines.append("| " + " | ".join([method_label] + cells) + " |")

    with open(args.out, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
