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

SECONDS=0
"${EXE}" --montecarlo "${SAMPLES}" --threads "${THREADS}" --seed "${SEED}" --csv "${OUT_DIR}/montecarlo.csv" > "${OUT_DIR}/montecarlo.log"

echo "${SECONDS}" > "${OUT_DIR}/runtime_seconds.txt"

echo "Wrote Monte Carlo CSV to ${OUT_DIR}/montecarlo.csv"
