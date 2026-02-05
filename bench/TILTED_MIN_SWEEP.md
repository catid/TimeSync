# Tilted-Min + Shadow Promotion Sweep

Command:

```
SEEDS=5 THREADS=64 bench/run_tilted_shadow_sweep.sh
```

Output:
`benchmarks/run_tilted_shadow_20260203_131213`

Key p95 offset results (us):

| method | E7_drift | E63_clock_step_1s_a_fwd | E67_clock_step_120s_a_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- |
| M4_TimeSync | 3458 | 500008 | 134218000 | 0 |
| M4_TimeSyncTilted | 4628 | 8 | 134218000 | 87923700 |
| M4_TimeSyncTilted:shadow_rtt | 4628 | 8 | 134218000 | 87923700 |

Notes:
- The shadow-promotion hybrid (`M4_TimeSyncTilted:shadow_rtt`) matched `M4_TimeSyncTilted` on drift and steps and did not improve E7 versus baseline.
- Large step recovery (120s) remains dominated by very large errors for both tilted variants.
