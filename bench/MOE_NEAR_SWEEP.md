# MoE Near-Floor Gating Sweep

Command:

```
SEEDS=5 THREADS=64 bench/run_moe_near_sweep.sh
```

Output:
`benchmarks/run_moe_near_20260203_131251`

Key p95 offset results (us):

| method | E53_randstep_normal | E7_drift |
| --- | --- | --- |
| M4_TimeSync | 1164130 | 3458 |
| M4_TimeSyncMoE | 1164140 | 4628 |
| M4_TimeSyncMoE:near0p1 | 1164140 | 4628 |

Notes:
- Near-floor gating did not improve MoE results on the drift or random-step cases.
- MoE variants remain worse than baseline on E7 (≈4628 vs 3458).
