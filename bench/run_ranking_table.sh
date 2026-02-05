#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

INPUT="${1:-}"
OUT="${2:-${ROOT_DIR}/bench/RANKING_TABLE.md}"
BASELINE="${BASELINE:-M4_TimeSync}"
TOLERANCE="${TOLERANCE:-0}"
TOP="${TOP:-10}"

if [[ -z "${INPUT}" ]]; then
  echo "Usage: $0 <run_dir|peer_bench.csv> [out.md]" >&2
  exit 1
fi

if [[ -d "${INPUT}" ]]; then
  CSV="${INPUT}/peer_bench.csv"
else
  CSV="${INPUT}"
fi

if [[ ! -f "${CSV}" ]]; then
  echo "Missing peer_bench.csv at ${CSV}" >&2
  exit 1
fi

python3 "${ROOT_DIR}/bench/report_composite_profiles.py" "${CSV}" \
  --out "${OUT}" \
  --baseline "${BASELINE}" \
  --tolerance "${TOLERANCE}" \
  --top "${TOP}"

echo "Wrote ${OUT}"
