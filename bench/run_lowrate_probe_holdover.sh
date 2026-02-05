#!/usr/bin/env bash
set -euo pipefail

OUT_DIR="${1:-lowrate_holdover}"
SCENARIO_FILTER="${2:-E7_}"
SEEDS="${3:-2}"

mkdir -p "$OUT_DIR"

rates=(0.1 0.2 0.5 1.0)
methods=("M1_Cristian" "M2_NTP" "M3_PTP" "M4_TimeSyncProbe")

for rate in "${rates[@]}"; do
  for method in "${methods[@]}"; do
    out_csv="$OUT_DIR/peer_bench_${method}_rate${rate}.csv"
    echo "Running $method at ${rate} Hz (scenario filter: $SCENARIO_FILTER)"
    ./build/peer_bench \
      --scenario "$SCENARIO_FILTER" \
      --method-exact "$method" \
      --probe-rate-hz "$rate" \
      --seeds "$SEEDS" \
      --out "$out_csv"
  done
done

python3 bench/merge_peer_bench.py "$OUT_DIR/peer_bench_holdover.csv" "$OUT_DIR"/peer_bench_*.csv --dedupe
python3 bench/report_lowrate_holdover.py "$OUT_DIR/peer_bench_holdover.csv" \
  --out "$OUT_DIR/lowrate_holdover.md"

echo "Wrote $OUT_DIR/peer_bench_holdover.csv and $OUT_DIR/lowrate_holdover.md"
