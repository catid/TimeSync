# Adaptive Bins Sweep

Command:

```
SEEDS=5 THREADS=64 bench/run_adaptive_bins_sweep.sh
```

Output:
`benchmarks/run_adaptive_bins_20260203_131242`

Key p95 offset results (us):

| method | E0_noise_floor | E4_loss_burst | E5_reorder_dup | E6_path_change | E7_drift |
| --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 160 | 8 | 3458 |
| M4_TimeSyncAdaptiveBins | 0 | 16 | 128 | 8 | 3908 |
| M4_TimeSyncAdaptiveBins:fast2 | 0 | 16 | 128 | 8 | 4060 |

Notes:
- Adaptive bins reduce E5 slightly vs baseline, but E7 drift worsens (3908–4060 vs 3458).
- E0/E4/E6 stay on-baseline; E7 regression means this variant is not a net improvement.
