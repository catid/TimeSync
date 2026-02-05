#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

PATTERN='steady_clock|system_clock|sleep_for|clock_gettime|gettimeofday'

mapfile -t MATCHES < <(rg -n "${PATTERN}" tests src inc || true)

if [[ ${#MATCHES[@]} -eq 0 ]]; then
  echo "No wallclock references found (unexpected)."
  exit 1
fi

# Expected references: only in tests/experiments.cpp and fixed count.
EXPECTED_FILE="tests/experiments.cpp"
EXPECTED_COUNT=6

BAD=0
COUNT=0
for line in "${MATCHES[@]}"; do
  file=${line%%:*}
  if [[ "${file}" != "${EXPECTED_FILE}" ]]; then
    echo "Unexpected wallclock reference: ${line}"
    BAD=1
  else
    COUNT=$((COUNT + 1))
  fi
done

if [[ ${COUNT} -ne ${EXPECTED_COUNT} ]]; then
  echo "Wallclock reference count changed in ${EXPECTED_FILE}: ${COUNT} (expected ${EXPECTED_COUNT})"
  BAD=1
fi

if [[ ${BAD} -ne 0 ]]; then
  echo "Wallclock audit FAILED."
  exit 1
fi

echo "Wallclock audit OK (${EXPECTED_FILE} count=${COUNT})."
