#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/quantile_swap_$(date +%Y%m%d_%H%M%S)}"

SEEDS="${SEEDS:-3}"
SCENARIO_FILTER="${SCENARIO_FILTER:-}"

mkdir -p "${OUT_DIR}"

CMD=("${ROOT_DIR}/build/peer_bench" --grid --seeds "${SEEDS}" --out "${OUT_DIR}/peer_bench.csv")
CMD+=(--method "M4_TimeSyncQuantileSwap")

if [[ -n "${SCENARIO_FILTER}" ]]; then
  CMD+=(--scenario "${SCENARIO_FILTER}")
fi

"${CMD[@]}"

python3 "${ROOT_DIR}/bench/report_quantile_swap_sweep.py" "${OUT_DIR}/peer_bench.csv" \
  --out "${OUT_DIR}/quantile_swap_sweep.csv"

printf "\nQuantile swap sweep outputs: %s\n" "${OUT_DIR}"
