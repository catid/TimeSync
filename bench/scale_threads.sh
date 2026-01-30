#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/experiments"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_scale_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SAMPLES="${SAMPLES:-10000}"
SEED="${SEED:-7777}"
THREADS_LIST="${THREADS_LIST:-1 2 4 8 16 32}"

printf "threads,runtime_seconds\n" > "${OUT_DIR}/scale.csv"

for t in ${THREADS_LIST}; do
  if [[ "$t" -gt "$(nproc)" ]]; then
    continue
  fi
  SECONDS=0
  "${EXE}" --montecarlo "${SAMPLES}" --threads "$t" --seed "${SEED}" --csv "${OUT_DIR}/mc_${t}.csv" > "${OUT_DIR}/mc_${t}.log"
  echo "$t,${SECONDS}" >> "${OUT_DIR}/scale.csv"
  sleep 0.5

done

echo "Wrote scaling results to ${OUT_DIR}/scale.csv"
