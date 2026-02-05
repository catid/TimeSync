# Mixture-of-Experts (MoE) Sweep

Goal: evaluate M4_TimeSyncMoE (anchor + tracker) with stability gating.

## Run

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncMoE' \
  bench/run_peer_bench.sh benchmarks/run_moe_20260203_004000
```

## Results (p95 poll error, median across seeds)

From `benchmarks/run_moe_20260203_004000/peer_bench.csv`:

M4_TimeSyncMoE (base):
- E0_noise_floor: 0.0 / 499000.0 (avg 249500.0) **regression**
- E4_loss_burst: 16.0 / 16.0 (avg 16.0)
- E5_reorder_dup: 56.0 / 48.0 (avg 52.0)
- E7_drift: 4620.0 / 4372.0 (avg 4496.0)

M4_TimeSyncMoE:xor:
- E0_noise_floor: 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: 16.0 / 16.0 (avg 16.0)
- E5_reorder_dup: 56.0 / 48.0 (avg 52.0)
- E7_drift: 4620.0 / 4372.0 (avg 4496.0)

## Baseline reference

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Takeaways

## Tuning follow-up

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncMoE' \
  bench/run_peer_bench.sh benchmarks/run_moe_tune_20260203_024000
```

From `benchmarks/run_moe_tune_20260203_024000/peer_bench.csv`:

- noguard_xor: E0 ok (0), E7 avg p95 4604 (still worse than baseline).
- resid200/guard1ms did not fix E0 regression in base variant.

## Takeaways

- XOR gating is still required to avoid E0 regression; residual clamp did not fix base MoE.\n- None of the tuned variants improved E7 meaningfully; tracker likely not updating under current gates or output blending.\n- Further work should focus on why BA spikes (499000) occur and why skew remains ~0 in E7.
