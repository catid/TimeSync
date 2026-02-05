#!/usr/bin/env bash
set -euo pipefail

OUT_DIR="${1:-step_size_sweep}"
SEEDS="${2:-2}"
SCENARIO_FILTER="${3:-clock_step_}"

mkdir -p "$OUT_DIR"

methods=("M4_TimeSyncDDAC" "M4_TimeSyncShadow")

for method in "${methods[@]}"; do
  out_csv="$OUT_DIR/peer_bench_${method}.csv"
  echo "Running $method on scenarios matching $SCENARIO_FILTER"
  ./build/peer_bench \
    --scenario "$SCENARIO_FILTER" \
    --method-exact "$method" \
    --seeds "$SEEDS" \
    --out "$out_csv"
done

python3 bench/merge_peer_bench.py "$OUT_DIR/peer_bench_step_sizes.csv" \
  "$OUT_DIR"/peer_bench_*.csv --dedupe

python3 bench/report_step_size_sweep.py "$OUT_DIR/peer_bench_step_sizes.csv" \
  --out "$OUT_DIR/step_size_summary.md"

echo "Wrote $OUT_DIR/peer_bench_step_sizes.csv and $OUT_DIR/step_size_summary.md"
