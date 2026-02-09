#!/usr/bin/env python3
"""Benchmark comparison and regression gate tool.

Compares two benchmark CSV files (baseline vs candidate) and reports:
1. Per-method statistical comparison with confidence intervals
2. Per-scenario regression detection
3. Overall pass/fail for CI integration

Usage:
    python3 tools/bench_compare.py baseline.csv candidate.csv [--threshold 10]
    python3 tools/bench_compare.py results.csv --summary  # Just summarize one file
"""

import csv
import sys
import math
from collections import defaultdict


def safe_float(v):
    try:
        x = float(v)
        if math.isnan(x) or math.isinf(x):
            return None
        return x
    except (ValueError, TypeError):
        return None


def load_data(path):
    rows = []
    with open(path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)
    return rows


def median(vals):
    s = sorted(vals)
    n = len(s)
    if n == 0:
        return 0
    return s[n // 2] if n % 2 == 1 else (s[n // 2 - 1] + s[n // 2]) / 2


def mean(vals):
    return sum(vals) / len(vals) if vals else 0


def percentile(vals, p):
    s = sorted(vals)
    n = len(s)
    if n == 0:
        return 0
    return s[min(int(p * (n - 1)), n - 1)]


def stdev(vals):
    if len(vals) < 2:
        return 0
    m = mean(vals)
    return math.sqrt(sum((v - m) ** 2 for v in vals) / (len(vals) - 1))


def ci95(vals):
    """95% confidence interval half-width using t-distribution approximation."""
    n = len(vals)
    if n < 2:
        return float('inf')
    se = stdev(vals) / math.sqrt(n)
    # t-value for 95% CI with df = n-1, approximate for df >= 4
    t_val = 2.776 if n <= 5 else 2.228 if n <= 10 else 2.045 if n <= 30 else 1.96
    return t_val * se


def extract_offset_p95(rows, method_filter=None):
    """Group by (scenario, method) -> list of avg(ab, ba) offset p95 values."""
    groups = defaultdict(list)
    for row in rows:
        m = row['method']
        if method_filter and method_filter not in m:
            continue
        ab = safe_float(row.get('offset_p95_ab_us', ''))
        ba = safe_float(row.get('offset_p95_ba_us', ''))
        if ab is not None and ba is not None:
            groups[(row['scenario'], m)].append((ab + ba) / 2)
    return groups


def summarize(path):
    """Print summary statistics for a single benchmark file."""
    rows = load_data(path)
    print(f"File: {path}")
    print(f"Total rows: {len(rows)}")

    groups = extract_offset_p95(rows)
    scenarios = sorted(set(k[0] for k in groups))
    methods = sorted(set(k[1] for k in groups))

    print(f"Scenarios: {len(scenarios)}")
    print(f"Methods: {len(methods)}")

    # Per-method ranking with confidence intervals
    method_scores = defaultdict(list)
    for (scenario, method), vals in groups.items():
        method_scores[method].append(median(vals))

    ranking = []
    for method, meds in method_scores.items():
        ranking.append((method, median(meds), mean(meds), ci95(meds), len(meds)))
    ranking.sort(key=lambda x: x[1])

    print(f"\n{'Method':<60} {'Med p95':>10} {'Mean p95':>10} {'CI95':>10} {'#Scen':>6}")
    print("-" * 98)
    for m, med, avg, ci, n in ranking[:30]:
        print(f"{m:<60} {med:>10.1f} {avg:>10.1f} +/-{ci:>7.1f} {n:>6}")
    print(f"\n... ({len(ranking)} methods total, showing top 30)")


def compare(baseline_path, candidate_path, threshold_pct):
    """Compare two benchmark files and report regressions."""
    base_rows = load_data(baseline_path)
    cand_rows = load_data(candidate_path)

    print(f"Baseline: {baseline_path} ({len(base_rows)} rows)")
    print(f"Candidate: {candidate_path} ({len(cand_rows)} rows)")

    base_groups = extract_offset_p95(base_rows)
    cand_groups = extract_offset_p95(cand_rows)

    # Compare per (scenario, method)
    regressions = []
    improvements = []
    unchanged = []

    keys = sorted(set(base_groups.keys()) & set(cand_groups.keys()))
    print(f"Common (scenario, method) pairs: {len(keys)}")

    for key in keys:
        base_vals = base_groups[key]
        cand_vals = cand_groups[key]

        base_med = median(base_vals)
        cand_med = median(cand_vals)

        if base_med <= 0:
            continue

        pct_change = (cand_med - base_med) / base_med * 100

        # Statistical significance: check if CI95 of difference excludes zero
        # Simple approach: non-overlapping CIs suggest significance
        base_ci = ci95(base_vals)
        cand_ci = ci95(cand_vals)

        entry = (key[0], key[1], base_med, cand_med, pct_change, base_ci, cand_ci)

        if pct_change > threshold_pct:
            regressions.append(entry)
        elif pct_change < -threshold_pct:
            improvements.append(entry)
        else:
            unchanged.append(entry)

    # Report
    print(f"\n{'='*80}")
    print(f"REGRESSION GATE (threshold: {threshold_pct}%)")
    print(f"{'='*80}")

    if regressions:
        regressions.sort(key=lambda x: -x[4])
        print(f"\nREGRESSIONS ({len(regressions)}):")
        print(f"{'Scenario':<35} {'Method':<35} {'Base':>8} {'Cand':>8} {'Change':>8}")
        print("-" * 96)
        for scen, meth, bm, cm, pct, bci, cci in regressions[:20]:
            print(f"{scen:<35} {meth:<35} {bm:>8.1f} {cm:>8.1f} {pct:>+7.1f}%")
    else:
        print("\nNo regressions detected.")

    if improvements:
        improvements.sort(key=lambda x: x[4])
        print(f"\nIMPROVEMENTS ({len(improvements)}):")
        print(f"{'Scenario':<35} {'Method':<35} {'Base':>8} {'Cand':>8} {'Change':>8}")
        print("-" * 96)
        for scen, meth, bm, cm, pct, bci, cci in improvements[:20]:
            print(f"{scen:<35} {meth:<35} {bm:>8.1f} {cm:>8.1f} {pct:>+7.1f}%")

    print(f"\nSummary: {len(regressions)} regressions, "
          f"{len(improvements)} improvements, "
          f"{len(unchanged)} unchanged")

    # Per-method aggregate comparison
    print(f"\n{'='*80}")
    print("PER-METHOD AGGREGATE COMPARISON")
    print(f"{'='*80}")

    base_method_scores = defaultdict(list)
    cand_method_scores = defaultdict(list)
    for (scen, meth), vals in base_groups.items():
        base_method_scores[meth].append(median(vals))
    for (scen, meth), vals in cand_groups.items():
        cand_method_scores[meth].append(median(vals))

    common_methods = sorted(set(base_method_scores.keys()) & set(cand_method_scores.keys()))

    print(f"\n{'Method':<50} {'Base Med':>10} {'Cand Med':>10} {'Change':>8} {'Base CI':>10} {'Cand CI':>10}")
    print("-" * 100)
    method_results = []
    for m in common_methods:
        bm = median(base_method_scores[m])
        cm = median(cand_method_scores[m])
        pct = (cm - bm) / bm * 100 if bm > 0 else 0
        bci = ci95(base_method_scores[m])
        cci = ci95(cand_method_scores[m])
        method_results.append((m, bm, cm, pct, bci, cci))

    method_results.sort(key=lambda x: x[1])
    for m, bm, cm, pct, bci, cci in method_results[:30]:
        flag = " ***" if abs(pct) > threshold_pct else ""
        print(f"{m:<50} {bm:>10.1f} {cm:>10.1f} {pct:>+7.1f}% +/-{bci:>6.1f} +/-{cci:>6.1f}{flag}")

    # Pass/fail
    passed = len(regressions) == 0
    print(f"\n{'PASS' if passed else 'FAIL'}: ", end="")
    if passed:
        print("No regressions exceed threshold.")
    else:
        print(f"{len(regressions)} regressions exceed {threshold_pct}% threshold.")

    return 0 if passed else 1


def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print("  bench_compare.py results.csv --summary")
        print("  bench_compare.py baseline.csv candidate.csv [--threshold 10]")
        sys.exit(1)

    if '--summary' in sys.argv:
        summarize(sys.argv[1])
        sys.exit(0)

    if len(sys.argv) < 3:
        print("Need two CSV files for comparison")
        sys.exit(1)

    threshold = 10.0
    for i, arg in enumerate(sys.argv):
        if arg == '--threshold' and i + 1 < len(sys.argv):
            threshold = float(sys.argv[i + 1])

    sys.exit(compare(sys.argv[1], sys.argv[2], threshold))


if __name__ == '__main__':
    main()
