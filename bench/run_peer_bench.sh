#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_peer_$(date +%Y%m%d_%H%M%S)}"

SEEDS="${SEEDS:-5}"
THREADS="${THREADS:-}" # empty -> auto
GRID="${GRID:-0}"
TRAIN_ONLY="${TRAIN_ONLY:-0}"
HOLDOUT_ONLY="${HOLDOUT_ONLY:-0}"
DURATION="${DURATION:-0}"
SCENARIO_FILTER="${SCENARIO_FILTER:-}"
METHOD_FILTER="${METHOD_FILTER:-}"
POLL_RATE_HZ="${POLL_RATE_HZ:-}"

mkdir -p "${OUT_DIR}"

CMD=("${ROOT_DIR}/build/peer_bench" --seeds "${SEEDS}" --out "${OUT_DIR}/peer_bench.csv")

if [[ "${GRID}" == "1" ]]; then
  CMD+=(--grid)
fi
if [[ "${TRAIN_ONLY}" == "1" ]]; then
  CMD+=(--train)
fi
if [[ "${HOLDOUT_ONLY}" == "1" ]]; then
  CMD+=(--holdout)
fi
if [[ -n "${THREADS}" ]]; then
  CMD+=(--threads "${THREADS}")
fi
if [[ -n "${SCENARIO_FILTER}" ]]; then
  CMD+=(--scenario "${SCENARIO_FILTER}")
fi
if [[ -n "${METHOD_FILTER}" ]]; then
  CMD+=(--method "${METHOD_FILTER}")
fi
if [[ "${DURATION}" != "0" ]]; then
  CMD+=(--duration "${DURATION}")
fi
if [[ -n "${POLL_RATE_HZ}" ]]; then
  CMD+=(--poll-rate-hz "${POLL_RATE_HZ}")
fi

"${CMD[@]}"

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

"${PYTHON}" "${ROOT_DIR}/bench/report_peer_bench.py" "${OUT_DIR}/peer_bench.csv" "${OUT_DIR}/peer_bench_report.pdf"

printf "\nPeer benchmark outputs: %s\n" "${OUT_DIR}"
