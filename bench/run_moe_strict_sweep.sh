#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_moe_strict_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"

SCENARIOS=(
  "E0_noise_floor"
  "E1_stationary_jitter"
  "E2_asymmetry_40ms"
  "E3_bufferbloat"
  "E4_loss_burst"
  "E5_reorder_dup"
  "E6_path_change"
  "E7_drift"
  "E14_clock_step_small"
  "E15_clock_step_large"
  "E18_clock_step_large_a_fwd"
)

METHODS=(
  "M4_TimeSync"
  "M4_TimeSyncMoE"
  "M4_TimeSyncMoE:strict"
  "M4_TimeSyncMoE:strict_xor"
)

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

CSV_LIST=()
for sc in "${SCENARIOS[@]}"; do
  for m in "${METHODS[@]}"; do
    tag="${m//[^a-zA-Z0-9_-]/_}"
    OUT_CSV="${OUT_DIR}/peer_bench_${sc}_${tag}.csv"
    CMD=("${EXE}" --seeds "${SEEDS}" --scenario "${sc}" --method "${m}" --out "${OUT_CSV}")
    if [[ "${GRID}" == "1" ]]; then
      CMD+=(--grid)
    fi
    if [[ -n "${THREADS}" ]]; then
      CMD+=(--threads "${THREADS}")
    fi
    "${CMD[@]}"
    CSV_LIST+=("${OUT_CSV}")
  done
done

MERGED="${OUT_DIR}/peer_bench.csv"
MERGE_CMD=("${PYTHON}" "${ROOT_DIR}/bench/merge_peer_bench.py" "${MERGED}" --dedupe)
MERGE_CMD+=("${CSV_LIST[@]}")
"${MERGE_CMD[@]}"

"${PYTHON}" "${ROOT_DIR}/bench/report_method_compare.py" "${MERGED}" \
  --baseline "M4_TimeSync" --tolerance 0 --require-poll --all-scenarios --score \
  --out "${OUT_DIR}/method_compare.md"

printf "\nMoE strict sweep outputs: %s\n" "${OUT_DIR}"
