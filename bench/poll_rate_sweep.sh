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

mkdir -p "${OUT_DIR}"
export OUT_DIR

for rate in ${POLL_RATES}; do
  out_csv="${OUT_DIR}/poll_rate_${rate}hz.csv"
  "${ROOT_DIR}/build/peer_bench" \
    --seeds "${SEEDS}" \
    --scenario "${SCENARIO}" \
    --duration "${DURATION}" \
    --method "${METHOD}" \
    --threads "${THREADS}" \
    --poll-rate-hz "${rate}" \
    --out "${out_csv}"
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
    with open(path, newline='') as f:
        for row in csv.DictReader(f):
            p95_ab.append(float(row['poll_time_err_p95_ab_us']))
            p95_ba.append(float(row['poll_time_err_p95_ba_us']))
    def stats(vals):
        if not vals:
            return 0.0, 0.0
        return sum(vals) / len(vals), max(vals)
    mean_ab, max_ab = stats(p95_ab)
    mean_ba, max_ba = stats(p95_ba)
    rows.append((rate_val, mean_ab, max_ab, mean_ba, max_ba, len(p95_ab)))

rows.sort(key=lambda r: r[0])
summary_path = os.path.join(out_dir, 'poll_rate_summary.csv')
with open(summary_path, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['poll_rate_hz', 'mean_p95_ab_us', 'max_p95_ab_us', 'mean_p95_ba_us', 'max_p95_ba_us', 'rows'])
    for row in rows:
        writer.writerow(row)

print(f"Wrote summary to {summary_path}")
PY

printf '\nPoll-rate sweep outputs: %s\n' "${OUT_DIR}"
