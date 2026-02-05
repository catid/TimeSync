#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_drift_step_mc_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-20}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-0}"
EXACT="${EXACT:-1}"
SCENARIO="${SCENARIO_OVERRIDE:-E31_drift_step_mc}"

METHODS=(
  "M4_TimeSync"
  "M4_TimeSyncShadow"
  "M4_TimeSyncStepReset"
)

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

CSV_LIST=()
for m in "${METHODS[@]}"; do
  tag="${m//[^a-zA-Z0-9_-]/_}"
  OUT_CSV="${OUT_DIR}/peer_bench_${SCENARIO}_${tag}.csv"
  if [[ "${EXACT}" == "1" ]]; then
    CMD=("${EXE}" --seeds "${SEEDS}" --scenario-exact "${SCENARIO}" --method-exact "${m}" --out "${OUT_CSV}")
  else
    CMD=("${EXE}" --seeds "${SEEDS}" --scenario "${SCENARIO}" --method "${m}" --out "${OUT_CSV}")
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

MERGED="${OUT_DIR}/peer_bench.csv"
MERGE_CMD=("${PYTHON}" "${ROOT_DIR}/bench/merge_peer_bench.py" "${MERGED}" --dedupe)
MERGE_CMD+=("${CSV_LIST[@]}")
"${MERGE_CMD[@]}"

"${PYTHON}" "${ROOT_DIR}/bench/report_clock_step_mc.py" "${MERGED}" \
  --scenario "${SCENARIO}" --out "${OUT_DIR}/drift_step_mc.md"

printf "\nDrift+step MC outputs: %s\n" "${OUT_DIR}"
