# Shadow + Skew Sweep

Goal: combine GSP (shadow promotion) with skew compensation on offset to keep E0–E6 strong and improve E7.

## Run

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncShadowSkew' \
  bench/run_peer_bench.sh benchmarks/run_shadow_skew_20260202_233200
```

## Results (p95 poll error, median across seeds)

From `benchmarks/run_shadow_skew_20260202_233200/peer_bench.csv` (base variant):

- E0_noise_floor: 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: 16.0 / 16.0 (avg 16.0)
- E5_reorder_dup: 56.0 / 48.0 (avg 52.0)
- E7_drift: 3470.0 / 3200.0 (avg 3335.0)

All variants (a0.05/a0.2/clamp50) produced identical E0–E7 metrics in this sweep.

## Baseline reference

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Takeaways

- Shadow+Skew improves E4/E5 vs baseline and slightly improves E7.
- Variant sweep did not separate; skew estimator may be too stable or dominated by GSP floor tracking.
- This variant looks like a promising candidate to keep for further tuning (e.g., enable XOR gate, adjust skew window).
