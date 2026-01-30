#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/experiments"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SEEDS="${SEEDS:-1 2 3 4 5}"
MATCH="${MATCH:-}"
ONLY="${ONLY:-}"
JOBS="${JOBS:-$(nproc)}"
THREADS="${THREADS:-}"
SECONDS=0

printf '%s\n' ${SEEDS} > "${OUT_DIR}/seeds.txt"

export EXE OUT_DIR MATCH ONLY THREADS

printf '%s\n' ${SEEDS} | xargs -I{} -P "${JOBS}" bash -c '
  seed="$1"
  csv="${OUT_DIR}/seed_${seed}.csv"
  log="${OUT_DIR}/seed_${seed}.log"
  args=(--seed "$seed" --csv "$csv")
  if [[ -n "${THREADS}" ]]; then
    args+=(--threads "${THREADS}")
  fi
  if [[ -n "${ONLY}" ]]; then
    args+=(--only "${ONLY}")
  fi
  if [[ -n "${MATCH}" ]]; then
    args+=(--match "${MATCH}")
  fi
  "${EXE}" "${args[@]}" > "${log}"
' _ {}

python3 "${ROOT_DIR}/bench/aggregate_csv.py" "${OUT_DIR}" "${OUT_DIR}/summary.csv"

echo "${SECONDS}" > "${OUT_DIR}/runtime_seconds.txt"
echo "Wrote per-seed CSVs and summary to ${OUT_DIR}"
