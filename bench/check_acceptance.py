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


def to_int(row, key, default=0):
    try:
        return int(float(row.get(key, default)))
    except ValueError:
        return default


def median(values):
    if not values:
        return 0.0
    return float(statistics.median(values))


def load_rows(path):
    with open(path, "r", newline="") as f:
        return list(csv.DictReader(f))


def scenario_key(name):
    if name.startswith("E7"):
        return "E7_drift"
    if name.startswith("E6"):
        return "E6_path_change"
    if name.startswith("E5"):
        return "E5_reorder_dup"
    if name.startswith("E4"):
        return "E4_loss_burst"
    if name.startswith("E3"):
        return "E3_bufferbloat"
    if name.startswith("E2"):
        return "E2_asymmetry"
    if name.startswith("E1"):
        return "E1_stationary_jitter"
    if name.startswith("E0"):
        return "E0_noise_floor"
    return None


def main():
    ap = argparse.ArgumentParser(description="Check minimal acceptance criteria against baseline.")
    ap.add_argument("csv_path", help="peer_bench.csv path")
    ap.add_argument("--baseline", required=True, help="Baseline method label (exact match)")
    ap.add_argument("--tolerance", type=float, default=0.0,
                    help="Allowed %% over baseline for E0-E6 (p95 poll time error)")
    ap.add_argument("--e7-improve-pct", type=float, default=0.0,
                    help="Required %% improvement vs baseline on E7 (0 = non-worse)")
    ap.add_argument("--dual-end", action="store_true",
                    help="Require AB/BA each within baseline tolerance (not just max)")
    ap.add_argument("--require-poll", action="store_true",
                    help="Require poll_time_err_valid_ab/ba = 1 for all scenarios")
    ap.add_argument("--require-mindelta-us", type=int, default=0,
                    help="Require mindelta_interval_us to match this value (0 = ignore)")
    ap.add_argument("--out", default="", help="Optional markdown output path")
    args = ap.parse_args()

    rows = load_rows(args.csv_path)
    if not rows:
        print("No rows in", args.csv_path)
        return 1

    grouped = defaultdict(list)
    for r in rows:
        sc = scenario_key(r.get("scenario", ""))
        if sc is None:
            continue
        key = (r["method"], r["estimator"], r["discipline"])
        grouped[key].append(r)

    baseline = {}
    baseline_ab = {}
    baseline_ba = {}
    baseline_poll_ok = {}
    for key, items in grouped.items():
        if key[0] != args.baseline:
            continue
        by_scenario = defaultdict(list)
        by_scenario_ab = defaultdict(list)
        by_scenario_ba = defaultdict(list)
        poll_ok = defaultdict(list)
        for r in items:
            sc = scenario_key(r["scenario"])
            if sc is None:
                continue
            val_ab = to_float(r, "poll_time_err_p95_ab_us")
            val_ba = to_float(r, "poll_time_err_p95_ba_us")
            by_scenario_ab[sc].append(val_ab)
            by_scenario_ba[sc].append(val_ba)
            by_scenario[sc].append(max(val_ab, val_ba))
            poll_ok[sc].append(min(to_int(r, "poll_time_err_valid_ab"),
                                   to_int(r, "poll_time_err_valid_ba")))
        for sc, vals in by_scenario.items():
            baseline[sc] = median(vals)
            baseline_ab[sc] = median(by_scenario_ab.get(sc, []))
            baseline_ba[sc] = median(by_scenario_ba.get(sc, []))
            baseline_poll_ok[sc] = min(poll_ok.get(sc, [1]))
        break

    if not baseline:
        print("Baseline method not found:", args.baseline)
        return 1

    rows_out = []
    for key, items in grouped.items():
        by_scenario = defaultdict(list)
        by_scenario_ab = defaultdict(list)
        by_scenario_ba = defaultdict(list)
        poll_ok = defaultdict(list)
        for r in items:
            sc = scenario_key(r["scenario"])
            if sc is None:
                continue
            val_ab = to_float(r, "poll_time_err_p95_ab_us")
            val_ba = to_float(r, "poll_time_err_p95_ba_us")
            by_scenario_ab[sc].append(val_ab)
            by_scenario_ba[sc].append(val_ba)
            by_scenario[sc].append(max(val_ab, val_ba))
            poll_ok[sc].append(min(to_int(r, "poll_time_err_valid_ab"),
                                   to_int(r, "poll_time_err_valid_ba")))
        scenario_vals = {sc: median(vals) for sc, vals in by_scenario.items()}
        scenario_vals_ab = {sc: median(vals) for sc, vals in by_scenario_ab.items()}
        scenario_vals_ba = {sc: median(vals) for sc, vals in by_scenario_ba.items()}
        poll_vals = {sc: min(poll_ok.get(sc, [1])) for sc in by_scenario.keys()}

        ok = True
        details = []
        for sc, base in baseline.items():
            if sc == "E7_drift":
                continue
            limit = base * (1.0 + args.tolerance / 100.0)
            if scenario_vals.get(sc, 0.0) > limit:
                ok = False
                details.append(f"{sc}>{limit:.0f}")
            if args.dual_end:
                base_ab = baseline_ab.get(sc, 0.0)
                base_ba = baseline_ba.get(sc, 0.0)
                limit_ab = base_ab * (1.0 + args.tolerance / 100.0) if base_ab > 0.0 else 0.0
                limit_ba = base_ba * (1.0 + args.tolerance / 100.0) if base_ba > 0.0 else 0.0
                if limit_ab > 0.0 and scenario_vals_ab.get(sc, 0.0) > limit_ab:
                    ok = False
                    details.append(f"{sc}_ab>{limit_ab:.0f}")
                if limit_ba > 0.0 and scenario_vals_ba.get(sc, 0.0) > limit_ba:
                    ok = False
                    details.append(f"{sc}_ba>{limit_ba:.0f}")
        e7_base = baseline.get("E7_drift", 0.0)
        e7_val = scenario_vals.get("E7_drift", 0.0)
        if e7_base > 0.0:
            e7_limit = e7_base * (1.0 - args.e7_improve_pct / 100.0)
            if e7_val > e7_limit:
                ok = False
                details.append(f"E7>{e7_limit:.0f}")
        if args.dual_end:
            e7_base_ab = baseline_ab.get("E7_drift", 0.0)
            e7_base_ba = baseline_ba.get("E7_drift", 0.0)
            if e7_base_ab > 0.0:
                e7_limit_ab = e7_base_ab * (1.0 - args.e7_improve_pct / 100.0)
                if scenario_vals_ab.get("E7_drift", 0.0) > e7_limit_ab:
                    ok = False
                    details.append(f"E7_ab>{e7_limit_ab:.0f}")
            if e7_base_ba > 0.0:
                e7_limit_ba = e7_base_ba * (1.0 - args.e7_improve_pct / 100.0)
                if scenario_vals_ba.get("E7_drift", 0.0) > e7_limit_ba:
                    ok = False
                    details.append(f"E7_ba>{e7_limit_ba:.0f}")

        if args.require_poll:
            for sc, okv in poll_vals.items():
                if okv <= 0:
                    ok = False
                    details.append(f"{sc}:no_poll")
        if args.require_mindelta_us:
            mindelta_vals = {to_int(r, "mindelta_interval_us", 0) for r in items}
            if (len(mindelta_vals) != 1) or (args.require_mindelta_us not in mindelta_vals):
                ok = False
                details.append(f"mindelta={sorted(mindelta_vals)}")

        rows_out.append((key, scenario_vals, ok, ", ".join(details)))

    rows_out.sort(key=lambda x: x[1].get("E7_drift", 0.0))

    lines = []
    lines.append("# Acceptance Check")
    lines.append("")
    lines.append(f"Source: {args.csv_path}")
    lines.append(f"Baseline: {args.baseline}")
    lines.append(f"E0-E6 tolerance: {args.tolerance:.1f}% over baseline")
    lines.append(f"E7 improvement: {args.e7_improve_pct:.1f}% vs baseline")
    lines.append(f"Dual-end enforcement: {'yes' if args.dual_end else 'no'}")
    lines.append(f"Require poll samples: {'yes' if args.require_poll else 'no'}")
    if args.require_mindelta_us:
        lines.append(f"Require MinDelta interval: {args.require_mindelta_us} us")
    lines.append("")
    header = "| method | estimator | discipline | E0 | E1 | E2 | E3 | E4 | E5 | E6 | E7 | pass | notes |"
    lines.append(header)
    lines.append("| " + " | ".join(["---"] * 11) + " |")
    for (method, est, disc), vals, ok, notes in rows_out:
        row = [
            method,
            est,
            disc,
            f"{vals.get('E0_noise_floor', 0.0):.0f}",
            f"{vals.get('E1_stationary_jitter', 0.0):.0f}",
            f"{vals.get('E2_asymmetry', 0.0):.0f}",
            f"{vals.get('E3_bufferbloat', 0.0):.0f}",
            f"{vals.get('E4_loss_burst', 0.0):.0f}",
            f"{vals.get('E5_reorder_dup', 0.0):.0f}",
            f"{vals.get('E6_path_change', 0.0):.0f}",
            f"{vals.get('E7_drift', 0.0):.0f}",
            "yes" if ok else "no",
            notes,
        ]
        lines.append("| " + " | ".join(row) + " |")

    if args.out:
        with open(args.out, "w") as f:
            f.write("\n".join(lines))
        print("Wrote", args.out)
    else:
        print("\n".join(lines))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
