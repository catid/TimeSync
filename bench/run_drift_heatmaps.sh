#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT_DIR}/build}"
EXE="${BUILD_DIR}/experiments"

if [[ ! -x "${EXE}" ]]; then
  cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi

OUT_DIR="${1:-${ROOT_DIR}/benchmarks/run_drift_mc_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "${OUT_DIR}"

SAMPLES_COARSE="${SAMPLES_COARSE:-2000}"
SAMPLES_DENSE="${SAMPLES_DENSE:-20000}"
THREADS="${THREADS:-$(nproc)}"
SEED="${SEED:-4242}"

AXES=(jitter_ms loss asymmetry_ms sync_s latency_ms rate_hz)

SECONDS=0

for axis in "${AXES[@]}"; do
  coarse_csv="${OUT_DIR}/coarse_${axis}.csv"
  dense_csv="${OUT_DIR}/dense_${axis}.csv"

  "${EXE}" --montecarlo "${SAMPLES_COARSE}" --threads "${THREADS}" --seed "${SEED}" --mc-axis "${axis}" --csv "${coarse_csv}" > "${OUT_DIR}/coarse_${axis}.log"

  range_file="${OUT_DIR}/range_${axis}.txt"
  /home/catid/timesync/TimeSync/bench/.venv/bin/python "${ROOT_DIR}/bench/choose_drift_ranges.py" "${coarse_csv}" "${axis}" > "${range_file}"

  # shellcheck source=/dev/null
  source "${range_file}"

  "${EXE}" --montecarlo "${SAMPLES_DENSE}" --threads "${THREADS}" --seed "$((SEED + 1))" \
    --mc-axis "${axis}" \
    --mc-drift-min "${drift_min}" --mc-drift-max "${drift_max}" \
    --mc-axis-min "${axis_min}" --mc-axis-max "${axis_max}" \
    --csv "${dense_csv}" > "${OUT_DIR}/dense_${axis}.log"

done

echo "${SECONDS}" > "${OUT_DIR}/runtime_seconds.txt"

/home/catid/timesync/TimeSync/bench/.venv/bin/python "${ROOT_DIR}/bench/report_drift_heatmaps.py" "${OUT_DIR}" "${OUT_DIR}/drift_heatmaps.pdf"

echo "Wrote drift heatmap report to ${OUT_DIR}/drift_heatmaps.pdf"
