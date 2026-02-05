#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/crossval_$(date +%Y%m%d_%H%M%S)}"

SEEDS="${SEEDS:-3}"
SCENARIO_FILTER="${SCENARIO_FILTER:-}"
METHOD_FILTER="${METHOD_FILTER:-}"

mkdir -p "${OUT_DIR}"

TRAIN_CMD=("${ROOT_DIR}/build/peer_bench" --grid --train --seeds "${SEEDS}" --out "${OUT_DIR}/train.csv")
HOLD_CMD=("${ROOT_DIR}/build/peer_bench" --grid --holdout --seeds "${SEEDS}" --out "${OUT_DIR}/holdout.csv")

if [[ -n "${SCENARIO_FILTER}" ]]; then
  TRAIN_CMD+=(--scenario "${SCENARIO_FILTER}")
  HOLD_CMD+=(--scenario "${SCENARIO_FILTER}")
fi
if [[ -n "${METHOD_FILTER}" ]]; then
  TRAIN_CMD+=(--method "${METHOD_FILTER}")
  HOLD_CMD+=(--method "${METHOD_FILTER}")
fi

"${TRAIN_CMD[@]}"
"${HOLD_CMD[@]}"

python3 "${ROOT_DIR}/bench/report_crossval_sweep.py" "${OUT_DIR}/train.csv" "${OUT_DIR}/holdout.csv" \
  --out "${OUT_DIR}/crossval_summary.csv"

printf "\nCrossval outputs: %s\n" "${OUT_DIR}"
