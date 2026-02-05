#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_video_sweep_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"
DURATION="${DURATION:-3}"

SCENARIOS=(
  "E10_video_baseline"
  "E11_video_congestion"
  "E12_video_bursty_loss"
  "E13_video_path_change"
  "E35_video_congestion_burst"
  "E36_video_handover_burst"
  "E46_video_latency_congestion"
  "E47_video_loss_reorder"
  "E48_video_congestion_step"
  "E49_video_lognormal_jitter"
  "E50_video_step_pathchange"
  "E51_video_drift_congestion"
  "E52_video_bimodal_pathchange"
  "E55_video_latency_spike_train"
  "E56_video_asym_jitter_ramp"
  "E57_video_pareto_jitter"
  "E58_video_spike_loss_reorder"
  "E71_video_latency_ramp"
  "E72_video_congestion_ramp"
  "E73_video_jitter_burst"
  "E74_video_path_bloat"
  "E75_video_asym_path_bloat"
  "E76_video_queue_sawtooth"
  "E77_video_asym_loss_burst"
  "E78_video_path_flap_jitter"
  "E80_video_congestion_pulses"
  "E82_video_rtt_random_walk_cellular"
  "E83_video_asym_bw_drop"
  "E84_video_multipath_reorder_burst"
  "E88_video_wifi_bursty"
  "E89_video_satlink_jitter"
  "E60_video_correlated_jitter"
  "E61_video_congestion_120ms"
  "E62_video_asym_spike_step"
)

METHODS=(
  "M4_TimeSync"
  "M4_TimeSyncShadow"
  "M4_TimeSyncShadow:step"
  "M4_TimeSyncAgeComp"
  "M4_TimeSyncDDAC:step"
  "M4_TimeSyncStepGuard"
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
  --baseline "M4_TimeSync" --tolerance 0 --require-poll --all-scenarios --score \
  --out "${OUT_DIR}/method_compare.md"

printf "\nVideo sweep outputs: %s\n" "${OUT_DIR}"
