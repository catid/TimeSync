#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_top_contenders_multiseed_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-10}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-0}"
BASELINE_METHOD="${BASELINE_METHOD:-M4_TimeSync}"

SCENARIO_LIST=(
  "E0_noise_floor"
  "E1_stationary_jitter"
  "E2_asymmetry_40ms"
  "E3_bufferbloat"
  "E4_loss_burst"
  "E5_reorder_dup"
  "E6_path_change"
  "E7_drift"
  "E63_clock_step_1s_a_fwd"
  "E64_clock_step_1s_a_back"
  "E65_clock_step_1s_b_fwd"
  "E66_clock_step_1s_b_back"
  "E67_clock_step_120s_a_fwd"
  "E68_clock_step_120s_a_back"
  "E69_clock_step_120s_b_fwd"
  "E70_clock_step_120s_b_back"
  "E88_video_wifi_bursty"
  "E89_video_satlink_jitter"
  "E90_sparse_traffic"
  "E91_clock_freeze_resume"
)

METHOD_LIST=(
  "M4_TimeSync"
  "M4_TimeSyncShadow"
  "M4_TimeSyncShadow:step"
  "M4_TimeSyncAgeComp"
  "M4_TimeSyncAgeComp:step"
  "M4_TimeSyncDDAC:step"
  "M4_TimeSyncStepGuard"
  "M4_TimeSyncTilted:shadow_rtt"
  "M4_TimeSyncAdaptiveBins"
)

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

printf "\nTop-contender multi-seed outputs: %s\n" "${OUT_DIR}"
