#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/peer_bench"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_ddac_agegap_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-$(nproc)}"
GRID="${GRID:-1}"
DURATION="${DURATION:-}"

SCENARIOS=(
  "E53_randstep_normal"
  "E54_drift_sign_flip"
)

METHODS=(
  "M4_TimeSync"
  "M4_TimeSyncDDAC"
  "M4_TimeSyncDDAC:age2s"
  "M4_TimeSyncDDAC:age10s"
  "M4_TimeSyncDDAC:clamp50"
  "M4_TimeSyncDDAC:clampiqr0p5"
  "M4_TimeSyncDDAC:clampiqr1p0"
  "M4_TimeSyncDDAC:signonly"
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
    CMD=("${EXE}" --seeds "${SEEDS}" --scenario-exact "${sc}" --method-exact "${m}" --out "${OUT_CSV}")
    if [[ "${GRID}" == "1" ]]; then
      CMD+=(--grid)
    fi
    if [[ -n "${DURATION}" ]]; then
      CMD+=(--duration "${DURATION}")
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

printf "\nDDAC age-gap sweep outputs: %s\n" "${OUT_DIR}"
