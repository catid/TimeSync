#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_poll_rate_$(date +%Y%m%d_%H%M%S)}"

POLL_RATES="${POLL_RATES:-2 5 10 20 50}"
SCENARIO="${SCENARIO:-E1}"
METHOD="${METHOD:-M2}"
DURATION="${DURATION:-1}"
SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-3}"
PROBE_RATE_HZ="${PROBE_RATE_HZ:-}"

mkdir -p "${OUT_DIR}"
export OUT_DIR

for rate in ${POLL_RATES}; do
  out_csv="${OUT_DIR}/poll_rate_${rate}hz.csv"
  args=(--seeds "${SEEDS}" --scenario "${SCENARIO}" --duration "${DURATION}" --method "${METHOD}" --threads "${THREADS}")
  if [[ -n "${PROBE_RATE_HZ}" ]]; then
    args+=(--probe-rate-hz "${PROBE_RATE_HZ}")
  fi
  args+=(--poll-rate-hz "${rate}" --out "${out_csv}")
  "${ROOT_DIR}/build/peer_bench" "${args[@]}"
  echo "Wrote ${out_csv}"
done

python3 - <<'PY'
import csv
import glob
import math
import os

out_dir = os.environ.get('OUT_DIR')
if not out_dir:
    raise SystemExit('OUT_DIR not set')

rows = []
for path in sorted(glob.glob(os.path.join(out_dir, 'poll_rate_*hz.csv'))):
    rate = path.split('poll_rate_')[-1].split('hz.csv')[0]
    try:
        rate_val = float(rate)
    except ValueError:
        continue
    p95_ab = []
    p95_ba = []
    cnt_ab = []
    cnt_ba = []
    overhead = []
    teleop_rms = []
    budget_margin = []
    over_budget = []
    has_counts = False
    with open(path, newline='') as f:
        for row in csv.DictReader(f):
            p95_ab.append(float(row['poll_time_err_p95_ab_us']))
            p95_ba.append(float(row['poll_time_err_p95_ba_us']))
            overhead.append(float(row.get('overhead_bps', 0.0)))
            teleop_rms.append(float(row.get('teleop_rms_error', 0.0)))
            budget_margin.append(float(row.get('budget_margin_bps', 0.0)))
            over_budget.append(float(row.get('over_budget', 0.0)))
            if 'poll_time_err_count_ab' in row:
                has_counts = True
                cnt_ab.append(float(row.get('poll_time_err_count_ab', 0.0)))
                cnt_ba.append(float(row.get('poll_time_err_count_ba', 0.0)))
    def stats(vals):
        if not vals:
            return 0.0, 0.0
        return sum(vals) / len(vals), max(vals)
    if has_counts:
        filt_p95_ab = [v for v, c in zip(p95_ab, cnt_ab) if c > 0.0]
        filt_p95_ba = [v for v, c in zip(p95_ba, cnt_ba) if c > 0.0]
        mean_ab, max_ab = stats(filt_p95_ab)
        mean_ba, max_ba = stats(filt_p95_ba)
        mean_cnt_ab, max_cnt_ab = stats(cnt_ab)
        mean_cnt_ba, max_cnt_ba = stats(cnt_ba)
        if mean_cnt_ab == 0.0 and mean_cnt_ba == 0.0:
            print(f"Warning: zero poll_time_err counts for rate {rate_val} in {os.path.basename(path)}")
    else:
        mean_ab, max_ab = stats(p95_ab)
        mean_ba, max_ba = stats(p95_ba)
        mean_cnt_ab = max_cnt_ab = mean_cnt_ba = max_cnt_ba = 0.0
    mean_overhead, max_overhead = stats(overhead)
    mean_teleop_rms, max_teleop_rms = stats(teleop_rms)
    mean_budget_margin, max_budget_margin = stats(budget_margin)
    if over_budget:
        pct_over_budget = 100.0 * sum(1 for v in over_budget if v > 0.5) / len(over_budget)
    else:
        pct_over_budget = 0.0
    rows.append((rate_val, mean_ab, max_ab, mean_ba, max_ba, mean_cnt_ab, max_cnt_ab, mean_cnt_ba, max_cnt_ba, mean_overhead, max_overhead, mean_teleop_rms, max_teleop_rms, mean_budget_margin, max_budget_margin, pct_over_budget, len(p95_ab)))

rows.sort(key=lambda r: r[0])
summary_path = os.path.join(out_dir, 'poll_rate_summary.csv')
with open(summary_path, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow([
        'poll_rate_hz',
        'mean_p95_ab_us',
        'max_p95_ab_us',
        'mean_p95_ba_us',
        'max_p95_ba_us',
        'mean_cnt_ab',
        'max_cnt_ab',
        'mean_cnt_ba',
        'max_cnt_ba',
        'mean_overhead_bps',
        'max_overhead_bps',
        'mean_teleop_rms',
        'max_teleop_rms',
        'mean_budget_margin_bps',
        'max_budget_margin_bps',
        'percent_over_budget',
        'rows',
    ])
    for row in rows:
        writer.writerow(row)

print(f"Wrote summary to {summary_path}")
PY

printf '\nPoll-rate sweep outputs: %s\n' "${OUT_DIR}"
