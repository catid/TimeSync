#!/usr/bin/env bash
set -euo pipefail

OUT_DIR="${1:-plausibility_sweep}"
SCENARIO_FILTER="${2:-E0_}"
SEEDS="${3:-3}"

mkdir -p "$OUT_DIR"

echo "Running plausibility sweep (scenario filter: $SCENARIO_FILTER)"
./build/peer_bench \
  --grid \
  --scenario "$SCENARIO_FILTER" \
  --method "plaus_" \
  --seeds "$SEEDS" \
  --out "$OUT_DIR/peer_bench_plaus.csv"

python3 bench/report_method_compare.py \
  "$OUT_DIR/peer_bench_plaus.csv" \
  --out "$OUT_DIR/plausibility_summary.md" \
  --all-scenarios

echo "Wrote $OUT_DIR/peer_bench_plaus.csv and $OUT_DIR/plausibility_summary.md"
