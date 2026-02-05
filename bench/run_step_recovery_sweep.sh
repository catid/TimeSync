#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_step_recovery_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"
DURATION="${DURATION:-}"

if [[ -n "${SCENARIOS_OVERRIDE:-}" ]]; then
  IFS=',' read -r -a SCENARIOS <<< "${SCENARIOS_OVERRIDE}"
else
  SCENARIOS=(
    "E14_clock_step_small"
    "E15_clock_step_large"
    "E16_clock_step_small_a_fwd"
    "E17_clock_step_small_a_back"
    "E18_clock_step_large_a_fwd"
    "E19_clock_step_large_a_back"
    "E20_clock_step_small_b_back"
    "E21_clock_step_large_b_back"
    "E63_clock_step_1s_a_fwd"
    "E64_clock_step_1s_a_back"
    "E65_clock_step_1s_b_fwd"
    "E66_clock_step_1s_b_back"
    "E67_clock_step_120s_a_fwd"
    "E68_clock_step_120s_a_back"
    "E69_clock_step_120s_b_fwd"
    "E70_clock_step_120s_b_back"
    "E45_clock_jump_small_a_fwd"
    "E45_clock_jump_small_a_back"
    "E45_clock_jump_large_b_fwd"
    "E45_clock_jump_large_b_back"
    "E53_randstep_normal"
  )
fi

METHODS=(
  "M4_TimeSync"
  "M4_TimeSyncShadow:step"
  "M4_TimeSyncStepGuard"
  "M4_TimeSyncStepGuard:binreset"
  "M4_TimeSyncDDAC:step"
)

POLL_RATES=(1 5 10)

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

CSV_LIST=()
for pr in "${POLL_RATES[@]}"; do
  for sc in "${SCENARIOS[@]}"; do
    for m in "${METHODS[@]}"; do
      tag="${m//[^a-zA-Z0-9_-]/_}"
      OUT_CSV="${OUT_DIR}/peer_bench_${sc}_${tag}_poll${pr}.csv"
      CMD=("${EXE}" --seeds "${SEEDS}" --scenario-exact "${sc}" --method-exact "${m}" --out "${OUT_CSV}" --poll-rate-hz "${pr}")
      if [[ -n "${DURATION}" ]]; then
        CMD+=(--duration "${DURATION}")
      fi
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
 done

MERGED="${OUT_DIR}/peer_bench.csv"
MERGE_CMD=("${PYTHON}" "${ROOT_DIR}/bench/merge_peer_bench.py" "${MERGED}" --dedupe)
MERGE_CMD+=("${CSV_LIST[@]}")
"${MERGE_CMD[@]}"

"${PYTHON}" "${ROOT_DIR}/bench/report_steps_compare.py" "${MERGED}" \
  --require-poll --methods "M4_TimeSync,M4_TimeSyncShadow:step,M4_TimeSyncStepGuard,M4_TimeSyncStepGuard:binreset,M4_TimeSyncDDAC:step" \
  --out "${OUT_DIR}/steps_compare.md"

printf "\nStep recovery sweep outputs: %s\n" "${OUT_DIR}"
