#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_top_contender_sweep_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"
DURATION="${DURATION:-}"
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
    "E7_drift"
    "E7_drift_ramp"
    "E61_video_congestion_120ms"
    "E14_clock_step_small"
    "E15_clock_step_large"
    "E18_clock_step_large_a_fwd"
    "E21_clock_step_large_b_back"
  )
fi

if [[ -n "${METHODS:-}" ]]; then
  IFS=',' read -r -a METHOD_LIST <<< "${METHODS}"
else
  METHOD_LIST=(
    "M4_TimeSync"
    "M4_TimeSyncShadow"
    "M4_TimeSyncShadow:step_xor_rtt"
    "M4_TimeSyncDDAC"
    "M4_TimeSyncDDAC:step"
    "M4_TimeSyncMoE:conf"
  )
fi

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

CSV_LIST=()
for sc in "${SCENARIO_LIST[@]}"; do
  for m in "${METHOD_LIST[@]}"; do
    tag="${m//[^a-zA-Z0-9_-]/_}"
    OUT_CSV="${OUT_DIR}/peer_bench_${sc}_${tag}.csv"
    CMD=("${EXE}" --seeds "${SEEDS}" --scenario "${sc}" --method "${m}" --out "${OUT_CSV}")
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

MERGED="${OUT_DIR}/peer_bench.csv"
MERGE_CMD=("${PYTHON}" "${ROOT_DIR}/bench/merge_peer_bench.py" "${MERGED}" --dedupe)
MERGE_CMD+=("${CSV_LIST[@]}")
"${MERGE_CMD[@]}"

"${PYTHON}" "${ROOT_DIR}/bench/report_method_compare.py" "${MERGED}" \
  --baseline "${BASELINE_METHOD}" --tolerance 0 --require-poll --all-scenarios --score \
  --out "${OUT_DIR}/method_compare.md"

"${PYTHON}" "${ROOT_DIR}/bench/report_composite_profiles.py" "${OUT_DIR}" \
  --baseline "${BASELINE_METHOD}" --tolerance 0 --top 12 \
  --out "${OUT_DIR}/composite_score.md"

"${PYTHON}" "${ROOT_DIR}/bench/report_pareto_frontier.py" "${MERGED}" \
  --baseline "${BASELINE_METHOD}" --out "${OUT_DIR}/pareto_frontier.md"

printf "\nTop-contender sweep outputs: %s\n" "${OUT_DIR}"
