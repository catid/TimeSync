# Age-Compensated Minima (ACM) Sweep

Goal: evaluate M4_TimeSyncAgeComp (min_long + skew*age) using CSE.

## Run

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncAgeComp' \
  bench/run_peer_bench.sh benchmarks/run_agecomp_20260203_000500
```

## Results (p95 poll error, median across seeds)

From `benchmarks/run_agecomp_20260203_000500/peer_bench.csv` (base variant):

- E0_noise_floor: 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: 16.0 / 16.0 (avg 16.0)
- E5_reorder_dup: 56.0 / 48.0 (avg 52.0)
- E7_drift: 4620.0 / 4372.0 (avg 4496.0)

All variants (age5s/age30s/b0.02/b0.10) produced identical E0–E7 metrics in this sweep.

## Baseline reference

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Takeaways

- ACM improves E4/E5 vs baseline but regresses E7 (avg 4496 vs 3361).
- Variant sweep did not separate; skew estimator likely not influencing mins yet.
