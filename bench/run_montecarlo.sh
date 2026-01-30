#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/experiments"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_mc_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SAMPLES="${SAMPLES:-1000}"
THREADS="${THREADS:-$(nproc)}"
SEED="${SEED:-12345}"
MC_FAMILY="${MC_FAMILY:-}"
MC_SEEDS="${MC_SEEDS:-10}"
EXTRA_ARGS="${EXTRA_ARGS:-}"
AUTO="${AUTO:-1}"
TARGET_MINUTES="${TARGET_MINUTES:-}"
CALIBRATE_SECONDS="${CALIBRATE_SECONDS:-}"

SECONDS=0
ARGS=(--threads "${THREADS}" --seed "${SEED}" --csv "${OUT_DIR}/montecarlo.csv")
if [[ "${AUTO}" == "1" ]]; then
  ARGS+=(--auto)
fi
if [[ -n "${TARGET_MINUTES}" ]]; then
  ARGS+=(--target-minutes "${TARGET_MINUTES}")
fi
if [[ -n "${CALIBRATE_SECONDS}" ]]; then
  ARGS+=(--calibrate-seconds "${CALIBRATE_SECONDS}")
fi
if [[ -n "${MC_FAMILY}" ]]; then
  ARGS+=(--mc-family "${MC_FAMILY}" --mc-seeds "${MC_SEEDS}")
else
  ARGS+=(--montecarlo "${SAMPLES}")
fi
if [[ -n "${EXTRA_ARGS}" ]]; then
  # shellcheck disable=SC2206
  ARGS+=(${EXTRA_ARGS})
fi

"${EXE}" "${ARGS[@]}" > "${OUT_DIR}/montecarlo.log"

echo "${SECONDS}" > "${OUT_DIR}/runtime_seconds.txt"

echo "Wrote Monte Carlo CSV to ${OUT_DIR}/montecarlo.csv"
