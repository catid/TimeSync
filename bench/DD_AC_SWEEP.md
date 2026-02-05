# DD-AC Sweep

Goal: evaluate M4_TimeSyncDDAC (drift-direction age compensation) vs baseline on E0/E4/E5/E7.

## Run

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncDDAC' \
  bench/run_peer_bench.sh benchmarks/run_ddac_20260203_013000
```

## Results (p95 poll error, median across seeds)

From `benchmarks/run_ddac_20260203_013000/peer_bench.csv`:

M4_TimeSyncDDAC (base):
- E0_noise_floor: 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: 16.0 / 0.0 (avg 8.0)
- E5_reorder_dup: 24.0 / 48.0 (avg 36.0)
- E7_drift: 5244.0 / 5188.0 (avg 5216.0)

M4_TimeSyncDDAC:noguard:
- E0_noise_floor: 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: 16.0 / 0.0 (avg 8.0)
- E5_reorder_dup: 24.0 / 48.0 (avg 36.0)
- E7_drift: 4476.0 / 4420.0 (avg 4448.0)

## Baseline reference

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Takeaways

- DD-AC improves E4/E5 relative to baseline but regresses E7 (avg 5216 base, 4448 noguard).
- Skew estimates are active (skew_p95 ~378 ppm in E7 for noguard), so the regression likely comes from sticky-direction gating/clamp logic.
- Needs further tuning of age gap/clamp or symmetric adjustment to avoid over/under-shoot in E7.
