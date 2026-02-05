#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/minreg_grid_$(date +%Y%m%d_%H%M%S)}"

SEEDS="${SEEDS:-3}"
SCENARIO_FILTER="${SCENARIO_FILTER:-}"
METHOD_FILTER="${METHOD_FILTER:-M4_TimeSyncMinReg}"

mkdir -p "${OUT_DIR}"

CMD=("${ROOT_DIR}/build/peer_bench" --grid --seeds "${SEEDS}" --out "${OUT_DIR}/peer_bench.csv")

if [[ -n "${SCENARIO_FILTER}" ]]; then
  CMD+=(--scenario "${SCENARIO_FILTER}")
fi
if [[ -n "${METHOD_FILTER}" ]]; then
  CMD+=(--method "${METHOD_FILTER}")
fi

"${CMD[@]}"

python3 "${ROOT_DIR}/bench/report_minreg_grid.py" "${OUT_DIR}/peer_bench.csv" \
  --out "${OUT_DIR}/minreg_grid_summary.csv"

printf "\nMinReg grid outputs: %s\n" "${OUT_DIR}"
