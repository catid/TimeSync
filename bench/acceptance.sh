#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_acceptance_$(date +%Y%m%d_%H%M%S)}"

SEEDS="${SEEDS:-5}"
THREADS="${THREADS:-}"
GRID="${GRID:-0}"
DURATION="${DURATION:-0}"
METHOD_FILTER="${METHOD_FILTER:-}"
POLL_RATE_HZ="${POLL_RATE_HZ:-}"
PROBE_RATE_HZ="${PROBE_RATE_HZ:-}"
MINDELTA_US="${MINDELTA_US:-1000000}"

BASELINE_METHOD="${BASELINE_METHOD:-M4_TimeSync}"
TOLERANCE_PCT="${TOLERANCE_PCT:-0}"
E7_IMPROVE_PCT="${E7_IMPROVE_PCT:-0}"
REQUIRE_POLL="${REQUIRE_POLL:-1}"
DEDUPE="${DEDUPE:-1}"

SCENARIOS="${SCENARIOS:-}"

mkdir -p "${OUT_DIR}"

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
fi

if [[ -z "${SCENARIOS}" ]]; then
  SCENARIO_LIST=(
    "E0_noise_floor"
    "E1_stationary_jitter"
    "E2_asymmetry_40ms"
    "E3_bufferbloat"
    "E4_loss_burst"
    "E5_reorder_dup"
    "E6_path_change"
    "E7_drift"
  )
else
  IFS=',' read -r -a SCENARIO_LIST <<< "${SCENARIOS}"
fi

CSV_LIST=()

for sc in "${SCENARIO_LIST[@]}"; do
  OUT_CSV="${OUT_DIR}/peer_bench_${sc}.csv"
  CSV_LIST+=("${OUT_CSV}")

  CMD=("${ROOT_DIR}/build/peer_bench" --seeds "${SEEDS}" --scenario "${sc}" --out "${OUT_CSV}")
  if [[ "${GRID}" == "1" ]]; then
    CMD+=(--grid)
  fi
  if [[ -n "${THREADS}" ]]; then
    CMD+=(--threads "${THREADS}")
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
  if [[ -n "${PROBE_RATE_HZ}" ]]; then
    CMD+=(--probe-rate-hz "${PROBE_RATE_HZ}")
  fi

  "${CMD[@]}"
done

MERGE_CMD=("${PYTHON}" "${ROOT_DIR}/bench/merge_peer_bench.py" "${OUT_DIR}/peer_bench.csv")
if [[ "${DEDUPE}" == "1" ]]; then
  MERGE_CMD+=(--dedupe)
fi
MERGE_CMD+=("${CSV_LIST[@]}")
"${MERGE_CMD[@]}"

ACCEPT_CMD=("${PYTHON}" "${ROOT_DIR}/bench/check_acceptance.py" "${OUT_DIR}/peer_bench.csv"
  --baseline "${BASELINE_METHOD}"
  --tolerance "${TOLERANCE_PCT}"
  --e7-improve-pct "${E7_IMPROVE_PCT}"
  --out "${OUT_DIR}/acceptance.md")
if [[ "${REQUIRE_POLL}" == "1" ]]; then
  ACCEPT_CMD+=(--require-poll)
fi
if [[ -n "${MINDELTA_US}" && "${MINDELTA_US}" != "0" ]]; then
  ACCEPT_CMD+=(--require-mindelta-us "${MINDELTA_US}")
fi
"${ACCEPT_CMD[@]}"

"${PYTHON}" "${ROOT_DIR}/bench/report_method_compare.py" "${OUT_DIR}/peer_bench.csv" \
  --baseline "${BASELINE_METHOD}" --tolerance "${TOLERANCE_PCT}" \
  --out "${OUT_DIR}/method_compare.md"

printf "\nAcceptance outputs: %s\n" "${OUT_DIR}"
