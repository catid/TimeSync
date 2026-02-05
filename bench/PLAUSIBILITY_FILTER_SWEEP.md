# TS24 Plausibility Filter Sweep

Goal: evaluate TS24 unwrap + plausibility bounds (NEG_EPS / DELTA_MAX / MIN_PHYS) and ensure it does not poison min-delta tracking.

## Runs

```
for sc in E0_noise_floor E4_loss_burst E5_reorder_dup E7_drift; do
  dur=20; if [ "$sc" = "E7_drift" ]; then dur=30; fi
  ./build/peer_bench --grid --scenario "$sc" --method "M4_TimeSync:plaus" \
    --seeds 1 --duration "$dur" --threads 8 \
    --out benchmarks/run_plaus_${sc}_20260202_190300/peer_bench.csv

  ./build/peer_bench --scenario "$sc" --method "M4_TimeSync" \
    --seeds 1 --duration "$dur" --threads 8 \
    --out benchmarks/run_plaus_${sc}_baseline_20260202_190300/peer_bench.csv
done
```

## Baseline (M4_TimeSync) p95 offset (AB/BA)

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 41.0 / 489937.0 (avg 244989.0)
- E5_reorder_dup: 92.0 / 77.0 (avg 84.5)
- E7_drift: 3434.0 / 3329.0 (avg 3381.5)

## Plausibility sweep outcome (NEG_EPS {0, 50us, 200us}, DELTA_MAX {1s, 2s, 5s}, MIN_PHYS {0, 25us, 50us})

Across all scenarios, the direction with negative delta (AB in these configs) was fully filtered out:

- E0_noise_floor: poll_time_err_valid_ab = 0 for all 27 variants; ts24_drops_ba = 1200 for every variant.
- E4_loss_burst: poll_time_err_valid_ab = 0 for all 27 variants; ts24_drops_ba = 895 for every variant.
- E5_reorder_dup: poll_time_err_valid_ab = 0 for all 27 variants; ts24_drops_ba = 1206 for every variant.
- E7_drift: poll_time_err_valid_ab = 0 for all 27 variants; ts24_drops_ba = 1799 for every variant.

Interpretation:
- With the current NEG_EPS sweep (≤200us), any negative deltas are rejected.
- Because the base scenarios use a 1s offset, one direction’s deltas are negative and are dropped entirely.
- Result: metrics in that direction are invalid; this filter is too strict for the current offset regime.

Next tuning idea:
- Expand NEG_EPS to tolerate expected offset (e.g., ≥1s), or bias the unwrap so deltas are centered around the expected offset before filtering.

## Script

```
bench/run_plausibility_sweep.sh <out_dir> <scenario_filter> <seeds>
```
