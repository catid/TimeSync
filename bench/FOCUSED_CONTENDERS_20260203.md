# Focused contenders sweep (E0–E7 + E53–E60)

- Run: `benchmarks/run_focused_contenders_20260203`
- Script: `bench/run_top_contenders.sh` with focused scenario list
- Methods: `M4_TimeSync`, `M4_TimeSyncShadow`, `M4_TimeSyncShadow:step`, `M4_TimeSyncAgeComp`, `M4_TimeSyncDDAC:step`, `M4_TimeSyncStepGuard`

## Key table (from method_compare.md)

| method | E0_noise_floor | E1_stationary_jitter | E2_asymmetry_40ms | E3_bufferbloat | E4_loss_burst | E5_reorder_dup | E6_path_change | E7_drift | E53_randstep_normal | E54_drift_sign_flip | E55_video_latency_spike_train | E56_video_asym_jitter_ramp | E57_video_pareto_jitter | E58_video_spike_loss_reorder | E59_path_flap | E60_video_correlated_jitter |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 20008 | 0 | 16 | 160 | 8 | 3448 | 837135 | 3440 | 4824 | 23256 | 10352 | 4696 | 22856 | 11960 |
| M4_TimeSyncShadow:step | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 3470 | 837143 | 3320 | 13920 | 19312 | 20464 | 14008 | 20160 | 27688 |
| M4_TimeSyncShadow | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 3470 | 837143 | 3320 | 13920 | 19312 | 20464 | 14008 | 20160 | 27688 |
| M4_TimeSyncAgeComp | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 4620 | 837143 | 4462 | 4824 | 6072 | 3168 | 4696 | 7496 | 5048 |
| M4_TimeSyncStepGuard | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 4620 | 837143 | 4462 | 4824 | 6072 | 3168 | 4696 | 7496 | 5048 |
| M4_TimeSyncDDAC:step | 0 | 8 | 20008 | 0 | 16 | 48 | 8 | 5244 | 837191 | 4552 | 4824 | 6072 | 10720 | 4696 | 7496 | 3936 |

## Observations
- Baseline/Shadow remain tight on E0–E6; Shadow variants are still slightly lower on E54 vs baseline.
- AgeComp/StepGuard improve the new video stressors (E56/E57/E60) relative to baseline/Shadow in this run.
- E53 random step remains a large error contributor across baseline‑safe methods.
