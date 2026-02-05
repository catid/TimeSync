# Short-Window Stats Validation

Goal: verify long-window bins + short-window stats (p10/IQR/near-hits) are computed and emitted in CSV.

## Run

```
mkdir -p benchmarks/run_short_stats_validation_20260202_190000
./build/peer_bench \
  --scenario E0_noise_floor \
  --method TimeSync \
  --seeds 1 \
  --duration 20 \
  --threads 8 \
  --out benchmarks/run_short_stats_validation_20260202_190000/peer_bench.csv
```

## Result snapshot (M4_TimeSync)

From `benchmarks/run_short_stats_validation_20260202_190000/peer_bench.csv`:

- short_p10_mean_ab_us: 1.002e+06
- short_iqr_mean_ab_us: 27.5556
- short_near_hits_mean_ab: 67.6667
- short_p10_mean_ba_us: 1.3322e+08
- short_iqr_mean_ba_us: 25.7778
- short_near_hits_mean_ba: 66.3333

Notes:
- p10/IQR/near_hits are non-zero and stable, confirming stats are being tracked.
- BA direction reflects TS24 wrap domain (large p10), which is expected under 1s offset.
