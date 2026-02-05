#!/usr/bin/env python3
import argparse
import csv
import os
import statistics
import math


MAX_ERR_US = 1.0e15


def to_float(row, key, default=0.0):
    try:
        return float(row.get(key, default))
    except ValueError:
        return default


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


def sanitize_positive(value):
    if value <= 0.0:
        return None
    if value > MAX_ERR_US:
        return None
    return value


def sanitize_ratio(value):
    if value < 0.0 or value > 1.0:
        return None
    return value


def group_medians_for(items, scenario_names, metric_ab, metric_ba, sanitize):
    scenario_vals = {}
    for sc in scenario_names:
        vals = [
            max(to_float(r, metric_ab), to_float(r, metric_ba))
            for r in items if r["scenario"] == sc
        ]
        cleaned = [sanitize(v) for v in vals]
        scenario_vals[sc] = median([v for v in cleaned if v is not None])
    group_vals = {}
    for sc, val in scenario_vals.items():
        g = scenario_group(sc)
        if g and val > 0.0:
            group_vals.setdefault(g, []).append(val)
    return {g: median(vals) for g, vals in group_vals.items()}


def group_medians_for_ratio(items, scenario_names, metric_ab, metric_ba):
    scenario_vals = {}
    for sc in scenario_names:
        vals = [
            max(to_float(r, metric_ab), to_float(r, metric_ba))
            for r in items if r["scenario"] == sc
        ]
        cleaned = [sanitize_ratio(v) for v in vals]
        scenario_vals[sc] = median([v for v in cleaned if v is not None])
    group_vals = {}
    for sc, val in scenario_vals.items():
        g = scenario_group(sc)
        if g and val is not None:
            group_vals.setdefault(g, []).append(val)
    return {g: median(vals) for g, vals in group_vals.items()}


def score_groups(groups, baseline_groups, weights):
    total = 0.0
    weight_sum = 0.0
    for g, w in weights.items():
        if g not in groups:
            continue
        denom = baseline_groups.get(g, 0.0)
        val = groups[g]
        ratio = val / denom if denom > 0.0 else val
        total += w * ratio
        weight_sum += w
    return total / weight_sum if weight_sum > 0.0 else None


def score_groups_geom(groups, baseline_groups, weights):
    total = 0.0
    weight_sum = 0.0
    for g, w in weights.items():
        if g not in groups:
            continue
        denom = baseline_groups.get(g, 0.0)
        val = groups[g]
        ratio = val / denom if denom > 0.0 else val
        if ratio <= 0.0:
            return None
        total += w * math.log(ratio)
        weight_sum += w
    if weight_sum <= 0.0:
        return None
    return math.exp(total / weight_sum)


def scenario_group(name):
    if name.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_")):
        return "E0-6"
    if name.startswith("E7_"):
        return "E7"
    if "video" in name:
        return "video"
    if ("step" in name) or ("clock" in name) or ("jump" in name):
        return "steps"
    return None


def parse_weights(spec):
    weights = {"E0-6": 3.0, "E7": 2.0, "video": 1.0, "steps": 1.0}
    if not spec:
        return weights
    for part in spec.split(","):
        if not part.strip():
            continue
        if "=" not in part:
            continue
        key, val = part.split("=", 1)
        key = key.strip()
        try:
            weights[key] = float(val.strip())
        except ValueError:
            continue
    return weights


def load_rows(paths, dedupe=True):
    rows = []
    seen = set()
    for path in paths:
        with open(path, "r", newline="") as f:
            reader = csv.DictReader(f)
            for row in reader:
                if dedupe:
                    key = (row.get("scenario", ""), row.get("method", ""),
                           row.get("estimator", ""), row.get("discipline", ""),
                           row.get("seed", ""))
                    if key in seen:
                        continue
                    seen.add(key)
                rows.append(row)
    return rows


def expand_inputs(inputs):
    csvs = []
    for path in inputs:
        if os.path.isdir(path):
            candidate = os.path.join(path, "peer_bench.csv")
            if os.path.isfile(candidate):
                csvs.append(candidate)
        elif os.path.isfile(path):
            csvs.append(path)
    return csvs


def main():
    ap = argparse.ArgumentParser(description="Aggregate weighted composite scores across sweeps.")
    ap.add_argument("inputs", nargs="+", help="peer_bench.csv files or run directories")
    ap.add_argument("--out", default="COMPOSITE_SCORECARD.md", help="Output markdown path")
    ap.add_argument("--baseline", default="M4_TimeSync", help="Baseline method label")
    ap.add_argument("--tolerance", type=float, default=0.0, help="Allowed % over baseline for E0-6")
    ap.add_argument("--weights", default="", help="Override weights: E0-6=3,E7=2,video=1,steps=1")
    ap.add_argument("--top", type=int, default=10, help="Top N methods to list")
    ap.add_argument("--require-groups", default="E0-6,E7,video,steps",
                    help="Comma list of scenario groups required for scoring")
    ap.add_argument("--penalty-weight", type=float, default=0.5,
                    help="Penalty weight for worst-case regression (geom score multiplier)")
    args = ap.parse_args()

    csvs = expand_inputs(args.inputs)
    if not csvs:
        print("No input CSVs found.")
        return 1

    rows = load_rows(csvs)
    if not rows:
        print("No rows loaded from inputs.")
        return 1

    scenario_names = sorted({r["scenario"] for r in rows})
    weights = parse_weights(args.weights)
    required_groups = [g.strip() for g in args.require_groups.split(",") if g.strip()]

    grouped = {}
    for r in rows:
        key = (r["method"], r["estimator"], r["discipline"])
        grouped.setdefault(key, []).append(r)

    baseline_scenario = {}
    baseline_groups_p95 = {}
    baseline_groups_p99 = {}
    baseline_groups_max = {}
    baseline_groups_var = {}
    baseline_groups_conv = {}
    for key, items in grouped.items():
        if key[0] != args.baseline:
            continue
        for sc in scenario_names:
            vals = [
                max(to_float(r, "poll_time_err_p95_ab_us"),
                    to_float(r, "poll_time_err_p95_ba_us"))
                for r in items if r["scenario"] == sc
            ]
            cleaned = [sanitize_err_us(v) for v in vals]
            baseline_scenario[sc] = median([v for v in cleaned if v is not None])
        groups = {}
        for sc, val in baseline_scenario.items():
            g = scenario_group(sc)
            if g and val > 0.0:
                groups.setdefault(g, []).append(val)
        baseline_groups_p95 = {g: median(vals) for g, vals in groups.items()}
        baseline_groups_p99 = group_medians_for(
            items, scenario_names,
            "poll_time_err_p99_ab_us", "poll_time_err_p99_ba_us",
            sanitize_err_us,
        )
        baseline_groups_max = group_medians_for(
            items, scenario_names,
            "poll_time_err_max_ab_us", "poll_time_err_max_ba_us",
            sanitize_err_us,
        )
        baseline_groups_var = group_medians_for(
            items, scenario_names,
            "poll_time_err_var_ab_us2", "poll_time_err_var_ba_us2",
            sanitize_positive,
        )
        baseline_groups_conv = group_medians_for(
            items, scenario_names,
            "converge_ab_s", "converge_ba_s",
            sanitize_positive,
        )
        break

    rows_out = []
    rows_out_p99 = []
    rows_out_max = []
    rows_out_var = []
    rows_out_conv = []
    rows_out_over20 = []
    rows_out_over50 = []
    rows_out_geom = []
    rows_out_geom_penalty = []
    for key, items in grouped.items():
        scenario_vals = {}
        for sc in scenario_names:
            vals = [
                max(to_float(r, "poll_time_err_p95_ab_us"),
                    to_float(r, "poll_time_err_p95_ba_us"))
                for r in items if r["scenario"] == sc
            ]
            cleaned = [sanitize_err_us(v) for v in vals]
            scenario_vals[sc] = median([v for v in cleaned if v is not None])
        group_vals = {}
        for sc, val in scenario_vals.items():
            g = scenario_group(sc)
            if g and val > 0.0:
                group_vals.setdefault(g, []).append(val)
        group_medians = {g: median(vals) for g, vals in group_vals.items()}
        group_medians_p99 = group_medians_for(
            items, scenario_names,
            "poll_time_err_p99_ab_us", "poll_time_err_p99_ba_us",
            sanitize_err_us,
        )
        group_medians_max = group_medians_for(
            items, scenario_names,
            "poll_time_err_max_ab_us", "poll_time_err_max_ba_us",
            sanitize_err_us,
        )
        group_medians_var = group_medians_for(
            items, scenario_names,
            "poll_time_err_var_ab_us2", "poll_time_err_var_ba_us2",
            sanitize_positive,
        )
        group_medians_conv = group_medians_for(
            items, scenario_names,
            "converge_ab_s", "converge_ba_s",
            sanitize_positive,
        )
        group_medians_over20 = group_medians_for_ratio(
            items, scenario_names,
            "poll_time_err_over20_ratio_ab", "poll_time_err_over20_ratio_ba",
        )
        group_medians_over50 = group_medians_for_ratio(
            items, scenario_names,
            "poll_time_err_over50_ratio_ab", "poll_time_err_over50_ratio_ba",
        )

        ok_baseline = True
        for sc, base in baseline_scenario.items():
            if not sc.startswith(("E0_", "E1_", "E2_", "E3_", "E4_", "E5_", "E6_")):
                continue
            if base <= 0.0:
                continue
            limit = base * (1.0 + args.tolerance / 100.0)
            if scenario_vals.get(sc, 0.0) > limit:
                ok_baseline = False
                break
        if "E0-6" in required_groups and group_medians.get("E0-6", 0.0) <= 0.0:
            ok_baseline = False

        missing_required = [g for g in required_groups if group_medians.get(g, 0.0) <= 0.0]
        if missing_required:
            score = None
            score_geom = None
            worst_ratio = None
        else:
            score = score_groups(group_medians, baseline_groups_p95, weights)
            score_geom = score_groups_geom(group_medians, baseline_groups_p95, weights)
            ratios = []
            for sc, val in scenario_vals.items():
                g = scenario_group(sc)
                if not g or g not in required_groups:
                    continue
                base = baseline_scenario.get(sc, 0.0)
                if base > 0.0 and val > 0.0:
                    ratios.append(val / base)
            worst_ratio = max(ratios) if ratios else None

        overhead = median([to_float(r, "overhead_bps") for r in items])
        rows_out.append((key, score, ok_baseline, overhead, group_medians))
        rows_out_p99.append((key, score_groups(group_medians_p99, baseline_groups_p99, weights),
                             ok_baseline, overhead, group_medians_p99))
        rows_out_max.append((key, score_groups(group_medians_max, baseline_groups_max, weights),
                             ok_baseline, overhead, group_medians_max))
        rows_out_var.append((key, score_groups(group_medians_var, baseline_groups_var, weights),
                             ok_baseline, overhead, group_medians_var))
        rows_out_conv.append((key, score_groups(group_medians_conv, baseline_groups_conv, weights),
                              ok_baseline, overhead, group_medians_conv))
        rows_out_over20.append((key, None, ok_baseline, overhead, group_medians_over20))
        rows_out_over50.append((key, None, ok_baseline, overhead, group_medians_over50))
        if score_geom is not None:
            rows_out_geom.append((key, score_geom, ok_baseline, overhead, group_medians, worst_ratio))
            penalty = 0.0
            if worst_ratio is not None and worst_ratio > 1.0:
                penalty = args.penalty_weight * (worst_ratio - 1.0)
            rows_out_geom_penalty.append(
                (key, score_geom * (1.0 + penalty), ok_baseline, overhead, group_medians, worst_ratio)
            )

    rows_out = [r for r in rows_out if r[1] is not None]
    rows_out_p99 = [r for r in rows_out_p99 if r[1] is not None]
    rows_out_max = [r for r in rows_out_max if r[1] is not None]
    rows_out_var = [r for r in rows_out_var if r[1] is not None]
    rows_out_conv = [r for r in rows_out_conv if r[1] is not None]
    rows_out.sort(key=lambda x: x[1])
    rows_out_p99.sort(key=lambda x: x[1])
    rows_out_max.sort(key=lambda x: x[1])
    rows_out_var.sort(key=lambda x: x[1])
    rows_out_conv.sort(key=lambda x: x[1])
    rows_out_geom.sort(key=lambda x: x[1])
    rows_out_geom_penalty.sort(key=lambda x: x[1])
    rows_out_over20.sort(key=lambda x: x[4].get("E0-6", 0.0))
    rows_out_over50.sort(key=lambda x: x[4].get("E0-6", 0.0))

    def render_table(rows_sel, title):
        lines = [f"## {title}", ""]
        lines.append("| method | estimator | discipline | score | E0-6 | E7 | video | steps | overhead_bps | baseline_safe |")
        lines.append("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |")
        for key, score, ok, overhead, groups in rows_sel:
            lines.append(
                f"| {key[0]} | {key[1]} | {key[2]} | {score:.3f} | "
                f"{groups.get('E0-6', 0.0):.0f} | {groups.get('E7', 0.0):.0f} | "
                f"{groups.get('video', 0.0):.0f} | {groups.get('steps', 0.0):.0f} | "
                f"{overhead:.0f} | {'yes' if ok else 'no'} |"
            )
        lines.append("")
        return lines

    def render_geom_table(rows_sel, title):
        lines = [f"## {title}", ""]
        lines.append("| method | estimator | discipline | score | worst_ratio | E0-6 | E7 | video | steps | overhead_bps | baseline_safe |")
        lines.append("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |")
        for key, score, ok, overhead, groups, worst_ratio in rows_sel:
            worst = worst_ratio if worst_ratio is not None else 0.0
            lines.append(
                f"| {key[0]} | {key[1]} | {key[2]} | {score:.3f} | {worst:.3f} | "
                f"{groups.get('E0-6', 0.0):.0f} | {groups.get('E7', 0.0):.0f} | "
                f"{groups.get('video', 0.0):.0f} | {groups.get('steps', 0.0):.0f} | "
                f"{overhead:.0f} | {'yes' if ok else 'no'} |"
            )
        lines.append("")
        return lines

    def render_ratio_table(rows_sel, title):
        lines = [f"## {title}", ""]
        lines.append("| method | estimator | discipline | E0-6 | E7 | video | steps |")
        lines.append("| --- | --- | --- | --- | --- | --- | --- |")
        def avg_ratio(groups):
            vals = [v for v in groups.values() if v is not None]
            return sum(vals) / len(vals) if vals else 0.0
        rows_sorted = sorted(rows_sel, key=lambda r: avg_ratio(r[4]))
        for key, _score, _ok, _overhead, groups in rows_sorted[:top_n]:
            lines.append(
                f"| {key[0]} | {key[1]} | {key[2]} | "
                f"{groups.get('E0-6', 0.0):.4f} | {groups.get('E7', 0.0):.4f} | "
                f"{groups.get('video', 0.0):.4f} | {groups.get('steps', 0.0):.4f} |"
            )
        lines.append("")
        return lines

    top_n = args.top
    baseline_safe = [r for r in rows_out if r[2]]
    top_all = rows_out[:top_n]
    top_safe = baseline_safe[:top_n]
    baseline_safe_p99 = [r for r in rows_out_p99 if r[2]]
    baseline_safe_max = [r for r in rows_out_max if r[2]]
    baseline_safe_var = [r for r in rows_out_var if r[2]]
    baseline_safe_conv = [r for r in rows_out_conv if r[2]]
    baseline_safe_geom = [r for r in rows_out_geom if r[2]]
    baseline_safe_geom_penalty = [r for r in rows_out_geom_penalty if r[2]]

    lines = []
    lines.append("# Composite Scorecard (Weighted)")
    lines.append("")
    lines.append("Sources:")
    for path in csvs:
        lines.append(f"- `{path}`")
    lines.append("")
    lines.append(f"Baseline: `{args.baseline}` (tolerance {args.tolerance:.1f}%)")
    lines.append(f"Weights: {', '.join(f'{k}={v:g}' for k, v in weights.items())}")
    lines.append(f"Required groups: {', '.join(required_groups)}")
    lines.append("")
    if baseline_groups_p95:
        lines.append("Baseline group medians (p95 us):")
        lines.append(
            f"- E0-6: {baseline_groups_p95.get('E0-6', 0.0):.0f}, "
            f"E7: {baseline_groups_p95.get('E7', 0.0):.0f}, "
            f"video: {baseline_groups_p95.get('video', 0.0):.0f}, "
            f"steps: {baseline_groups_p95.get('steps', 0.0):.0f}"
        )
        if baseline_groups_p99:
            lines.append(
                f"- p99 E0-6: {baseline_groups_p99.get('E0-6', 0.0):.0f}, "
                f"E7: {baseline_groups_p99.get('E7', 0.0):.0f}, "
                f"video: {baseline_groups_p99.get('video', 0.0):.0f}, "
                f"steps: {baseline_groups_p99.get('steps', 0.0):.0f}"
            )
        if baseline_groups_max:
            lines.append(
                f"- max E0-6: {baseline_groups_max.get('E0-6', 0.0):.0f}, "
                f"E7: {baseline_groups_max.get('E7', 0.0):.0f}, "
                f"video: {baseline_groups_max.get('video', 0.0):.0f}, "
                f"steps: {baseline_groups_max.get('steps', 0.0):.0f}"
            )
        if baseline_groups_var:
            lines.append(
                f"- var E0-6: {baseline_groups_var.get('E0-6', 0.0):.0f}, "
                f"E7: {baseline_groups_var.get('E7', 0.0):.0f}, "
                f"video: {baseline_groups_var.get('video', 0.0):.0f}, "
                f"steps: {baseline_groups_var.get('steps', 0.0):.0f}"
            )
        if baseline_groups_conv:
            lines.append(
                f"- conv_s E0-6: {baseline_groups_conv.get('E0-6', 0.0):.2f}, "
                f"E7: {baseline_groups_conv.get('E7', 0.0):.2f}, "
                f"video: {baseline_groups_conv.get('video', 0.0):.2f}, "
                f"steps: {baseline_groups_conv.get('steps', 0.0):.2f}"
            )
        lines.append("")

    lines += render_table(top_safe, f"Top {top_n} (baseline-safe)")
    lines += render_table(top_all, f"Top {top_n} (overall)")
    if rows_out_geom:
        lines.append(f"Penalty weight: {args.penalty_weight:g}")
        lines.append("")
        lines += render_geom_table(baseline_safe_geom[:top_n], f"Top {top_n} (geometric mean, baseline-safe)")
        lines += render_geom_table(rows_out_geom[:top_n], f"Top {top_n} (geometric mean, overall)")
    if rows_out_geom_penalty:
        lines += render_geom_table(
            baseline_safe_geom_penalty[:top_n],
            f"Top {top_n} (geometric + worst-case penalty, baseline-safe)",
        )
        lines += render_geom_table(
            rows_out_geom_penalty[:top_n],
            f"Top {top_n} (geometric + worst-case penalty, overall)",
        )
    if rows_out_p99:
        lines += render_table(baseline_safe_p99[:top_n], f"Top {top_n} by p99 (baseline-safe)")
        lines += render_table(rows_out_p99[:top_n], f"Top {top_n} by p99 (overall)")
    if rows_out_max:
        lines += render_table(baseline_safe_max[:top_n], f"Top {top_n} by max (baseline-safe)")
        lines += render_table(rows_out_max[:top_n], f"Top {top_n} by max (overall)")
    if rows_out_var:
        lines += render_table(baseline_safe_var[:top_n], f"Top {top_n} by variance (baseline-safe)")
        lines += render_table(rows_out_var[:top_n], f"Top {top_n} by variance (overall)")
    if rows_out_conv:
        lines += render_table(baseline_safe_conv[:top_n], f"Top {top_n} by convergence time (baseline-safe)")
        lines += render_table(rows_out_conv[:top_n], f"Top {top_n} by convergence time (overall)")
    if rows_out_over20:
        lines += render_ratio_table(
            [r for r in rows_out_over20 if r[2]],
            f"Top {top_n} by tail ratio >20ms (baseline-safe)"
        )
        lines += render_ratio_table(
            rows_out_over20,
            f"Top {top_n} by tail ratio >20ms (overall)"
        )
    if rows_out_over50:
        lines += render_ratio_table(
            [r for r in rows_out_over50 if r[2]],
            f"Top {top_n} by tail ratio >50ms (baseline-safe)"
        )
        lines += render_ratio_table(
            rows_out_over50,
            f"Top {top_n} by tail ratio >50ms (overall)"
        )

    with open(args.out, "w") as f:
        f.write("\n".join(lines))
    print("Wrote", args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
