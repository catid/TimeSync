#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_shadow_vargate_steps_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-2}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"

SCENARIOS=(
  "E0_noise_floor"
  "E4_loss_burst"
  "E5_reorder_dup"
  "E6_path_change"
  "E7_drift"
  "E14_clock_step_small"
  "E15_clock_step_large"
  "E16_clock_step_small_a_fwd"
  "E17_clock_step_small_a_back"
  "E18_clock_step_large_a_fwd"
  "E19_clock_step_large_a_back"
  "E20_clock_step_small_b_back"
  "E21_clock_step_large_b_back"
)

METHODS=(
  "M4_TimeSync"
  "M4_TimeSyncShadow"
  "M4_TimeSyncShadow:fast2"
  "M4_TimeSyncShadow:fast2_w1s"
  "M4_TimeSyncShadow:xor"
  "M4_TimeSyncShadow:rtt"
  "M4_TimeSyncShadow:xor_rtt"
  "M4_TimeSyncVarGate"
  "M4_TimeSyncVarGate:hold200ms"
  "M4_TimeSyncVarGate:hold500ms"
  "M4_TimeSyncVarGate:t50_v5k_w10"
  "M4_TimeSyncVarGate:t100_v20k_w20"
  "M4_TimeSyncVarGate:t150_v50k_w40"
  "M4_TimeSyncVarGate:robust"
  "M4_TimeSyncVarGate:t25_v2k_w10"
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

printf "\nShadow/VarGate step sweep outputs: %s\n" "${OUT_DIR}"
