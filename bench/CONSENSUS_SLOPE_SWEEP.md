# Consensus-Slope (Hough-style) Sweep

Goal: evaluate M4_TimeSyncConsensus vs baseline on E0/E4/E5/E7.

## Run

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncConsensus' \
  bench/run_peer_bench.sh benchmarks/run_consensus_20260202_232200
```

## Results (p95 poll error, median across seeds)

From `benchmarks/run_consensus_20260202_232200/peer_bench.csv` (base variant):

- E0_noise_floor: 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: 16.0 / 16.0 (avg 16.0)
- E5_reorder_dup: 56.0 / 48.0 (avg 52.0)
- E7_drift: 4620.0 / 4372.0 (avg 4496.0)

All consensus variants (k32/k128/cand5/cand20/bin1/bin5/p1/p10) produced identical E0–E7 metrics in this sweep.

## Baseline reference

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Tuning follow-up

```
SEEDS=3 THREADS=128 SCENARIO_FILTER='E' METHOD_FILTER='M4_TimeSyncConsensus' \
  bench/run_peer_bench.sh benchmarks/run_consensus_tune_20260203_010500
```

From `benchmarks/run_consensus_tune_20260203_010500/peer_bench.csv`:

- M4_TimeSyncConsensus:noguard: E7 avg p95 2325 (better than baseline 3361), but E4/E5 regress badly (E5 ~5000).
- Other variants (shortbase/loose) remained identical to base and did not improve E7.

## Takeaways

- Consensus-slope can improve E7 only when RTT guard is disabled, but that causes severe regressions in E4/E5/E6.
- With current guard thresholds, the estimator remains inactive and yields the “collapsed” baseline-like pattern.
