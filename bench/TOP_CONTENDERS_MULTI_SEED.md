# Top Contenders Multi-Seed Sweep

Command:

```
SEEDS=5 THREADS=64 bench/run_top_contenders_multiseed.sh
```

Output:
`benchmarks/run_top_contenders_multiseed_20260203_131301`

Key p95 offset results (us):

| method | E0_noise_floor | E4_loss_burst | E5_reorder_dup | E6_path_change | E7_drift | E63_clock_step_1s_a_fwd | E67_clock_step_120s_a_fwd | E88_video_wifi_bursty | E89_video_satlink_jitter | E90_sparse_traffic | E91_clock_freeze_resume |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 160 | 8 | 3458 | 500008 | 134218000 | 6480 | 16 | 888 | 3718820 |
| M4_TimeSyncShadow | 0 | 16 | 128 | 8 | 3454 | 8 | 134218000 | 8024 | 618616 | 488 | 6832810 |
| M4_TimeSyncShadow:step | 0 | 16 | 128 | 8 | 3454 | 8 | 134218000 | 8024 | 618616 | 488 | 6832810 |
| M4_TimeSyncAgeComp | 0 | 16 | 128 | 8 | 4628 | 8 | 134218000 | 3768 | 618616 | 488 | 9001940 |
| M4_TimeSyncAgeComp:step | 0 | 16 | 128 | 8 | 4628 | 8 | 134218000 | 3768 | 618616 | 488 | 9001940 |
| M4_TimeSyncDDAC:step | 0 | 16 | 48 | 8 | 5260 | 8 | 134218000 | 3768 | 618616 | 360 | 9001940 |
| M4_TimeSyncStepGuard | 0 | 16 | 128 | 8 | 4628 | 8 | 127109000 | 456 | 618616 | 488 | 9001940 |
| M4_TimeSyncTilted:shadow_rtt | 8 | 16 | 128 | 8 | 4628 | 8 | 134218000 | 3912 | 618616 | 488 | 9001940 |
| M4_TimeSyncAdaptiveBins | 0 | 16 | 128 | 8 | 3908 | 8 | 134218000 | 8024 | 618616 | 488 | 6832810 |

Notes:
- Baseline remains the best on E7 drift (3458 us). AdaptiveBins regresses E7 (3908 us) despite matching E0–E6.
- Shadow variants improve E5 but do not improve E7 or large-step cases. StepGuard improves E88 video wifi but still regresses E7.
- Large 120s step scenarios remain dominated by very large errors for all contenders tested here.
