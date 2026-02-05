#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_top_contenders_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"
BASELINE_METHOD="${BASELINE_METHOD:-M4_TimeSync}"

if [[ -n "${SCENARIOS:-}" ]]; then
  IFS=',' read -r -a SCENARIO_LIST <<< "${SCENARIOS}"
else
  SCENARIO_LIST=(
    "E0_noise_floor"
    "E1_stationary_jitter"
    "E2_asymmetry_40ms"
    "E3_bufferbloat"
    "E4_loss_burst"
    "E5_reorder_dup"
    "E6_path_change"
    "E59_path_flap"
    "E7_drift"
    "E7_drift_ramp"
    "E32_asym_loss_reorder_ba"
    "E33_lowrate_step"
    "E34_asym_drift"
    "E35_video_congestion_burst"
    "E36_video_handover_burst"
    "E61_video_congestion_120ms"
    "E37_asym_drift_step"
    "E10_video_baseline"
    "E11_video_congestion"
    "E12_video_bursty_loss"
    "E13_video_path_change"
    "E14_clock_step_small"
    "E15_clock_step_large"
    "E16_clock_step_small_a_fwd"
    "E17_clock_step_small_a_back"
    "E18_clock_step_large_a_fwd"
    "E19_clock_step_large_a_back"
    "E20_clock_step_small_b_back"
    "E21_clock_step_large_b_back"
  )
fi

if [[ -n "${METHOD_FILTERS:-}" ]]; then
  # space-separated list
  read -r -a METHOD_LIST <<< "${METHOD_FILTERS}"
else
  METHOD_LIST=(
    "M4_TimeSync"
    "M4_TimeSyncShadow"
    "M4_TimeSyncShadow:step"
    "M4_TimeSyncAgeComp"
    "M4_TimeSyncDDAC:step"
    "M4_TimeSyncStepGuard"
  )
fi

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

CSV_LIST=()
for sc in "${SCENARIO_LIST[@]}"; do
  for mf in "${METHOD_LIST[@]}"; do
    tag="${mf//[^a-zA-Z0-9_-]/_}"
    OUT_CSV="${OUT_DIR}/peer_bench_${sc}_${tag}.csv"
    CMD=("${EXE}" --seeds "${SEEDS}" --scenario "${sc}" --method "${mf}" --out "${OUT_CSV}")
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
  --baseline "${BASELINE_METHOD}" --tolerance 0 --require-poll --all-scenarios --score \
  --out "${OUT_DIR}/method_compare.md"

printf "\nTop-contender sweep outputs: %s\n" "${OUT_DIR}"
