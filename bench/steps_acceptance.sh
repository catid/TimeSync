#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_steps_acceptance_$(date +%Y%m%d_%H%M%S)}"

SEEDS="${SEEDS:-3}"
THREADS="${THREADS:-}"
GRID="${GRID:-0}"
METHOD_FILTER="${METHOD_FILTER:-}"
POLL_RATE_HZ="${POLL_RATE_HZ:-}"
PROBE_RATE_HZ="${PROBE_RATE_HZ:-}"
MINDELTA_US="${MINDELTA_US:-1000000}"
REQUIRE_POLL="${REQUIRE_POLL:-1}"

SCENARIO_LIST=(
  "E14_clock_step_small"
  "E15_clock_step_large"
  "E16_clock_step_small_a_fwd"
  "E17_clock_step_small_a_back"
  "E18_clock_step_large_a_fwd"
  "E19_clock_step_large_a_back"
  "E20_clock_step_small_b_back"
  "E21_clock_step_large_b_back"
)

mkdir -p "${OUT_DIR}"

if [[ -x "${ROOT_DIR}/bench/.venv/bin/python" ]]; then
  PYTHON="${ROOT_DIR}/bench/.venv/bin/python"
else
  PYTHON=python3
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
  if [[ -n "${POLL_RATE_HZ}" ]]; then
    CMD+=(--poll-rate-hz "${POLL_RATE_HZ}")
  fi
  if [[ -n "${PROBE_RATE_HZ}" ]]; then
    CMD+=(--probe-rate-hz "${PROBE_RATE_HZ}")
  fi
  "${CMD[@]}"
done

MERGED="${OUT_DIR}/peer_bench.csv"
MERGE_CMD=("${PYTHON}" "${ROOT_DIR}/bench/merge_peer_bench.py" "${MERGED}" --dedupe)
MERGE_CMD+=("${CSV_LIST[@]}")
"${MERGE_CMD[@]}"

STEP_REPORT="${OUT_DIR}/steps_compare.md"
REPORT_CMD=("${PYTHON}" "${ROOT_DIR}/bench/report_steps_compare.py" "${MERGED}" --out "${STEP_REPORT}")
if [[ "${REQUIRE_POLL}" == "1" ]]; then
  REPORT_CMD+=(--require-poll)
fi
"${REPORT_CMD[@]}"

ACCEPT_CMD=("${PYTHON}" "${ROOT_DIR}/bench/check_acceptance.py" "${MERGED}"
  --baseline "M4_TimeSync" --tolerance 0 --e7-improve-pct 0 --out "${OUT_DIR}/acceptance.md")
if [[ "${REQUIRE_POLL}" == "1" ]]; then
  ACCEPT_CMD+=(--require-poll)
fi
if [[ -n "${MINDELTA_US}" && "${MINDELTA_US}" != "0" ]]; then
  ACCEPT_CMD+=(--require-mindelta-us "${MINDELTA_US}")
fi
"${ACCEPT_CMD[@]}"

printf "\nStep acceptance outputs: %s\n" "${OUT_DIR}"
