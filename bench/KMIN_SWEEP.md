# KMin / KBest Sweep

Goal: evaluate M4_TimeSyncKMin and M4_TimeSyncKBest vs baseline on E0/E4/E5/E7.

## Run

```
SEEDS=2 GRID=1 THREADS=$(nproc) bench/run_kmin_kbest_sweep.sh
```

## Results (p95 poll error, median across seeds)

From `benchmarks/run_kmin_kbest_20260203_071758/peer_bench.csv` (base variants, p95 poll error):

M4_TimeSyncKMin:
- E0_noise_floor: 0
- E4_loss_burst: 32
- E5_reorder_dup: 204
- E7_drift: 4584
- E7_drift_25ppm: 2295
- E7_drift_100ppm: 9216
- E7_drift_200ppm: 18352

M4_TimeSyncKBest:
- E0_noise_floor: 0
- E4_loss_burst: 24
- E5_reorder_dup: 40
- E7_drift: 4592
- E7_drift_25ppm: 2302
- E7_drift_100ppm: 9236
- E7_drift_200ppm: 18472

Variants (k3/k5/n3) did not improve E7 in this sweep; KMin variants worsened E5.

## Baseline reference

From `benchmarks/run_kmin_kbest_20260203_071758/method_compare.md` (baseline):

- E0_noise_floor: 0
- E4_loss_burst: 24
- E5_reorder_dup: 136
- E7_drift: 3480
- E7_drift_25ppm: 1659
- E7_drift_100ppm: 6917
- E7_drift_200ppm: 13480

## Takeaways

- KMin (k=2+) regresses E5 and E7 vs baseline.
- KBest improves E5 but still regresses E7 drift (stale-eject gating not firing often enough).
