# Top contenders on new video scenarios (E55–E58)

- Run: `benchmarks/run_top_video_new_20260203`
- Script: `bench/run_top_contenders.sh` with E55–E58

## Key table (from method_compare.md)

| method | E55_video_latency_spike_train | E56_video_asym_jitter_ramp | E57_video_pareto_jitter | E58_video_spike_loss_reorder |
| --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step | 13920 | 19312 | 20464 | 14008 |
| M4_TimeSync | 4824 | 23256 | 10352 | 4696 |
| M4_TimeSyncDDAC:step | 4824 | 6072 | 10720 | 4696 |
| M4_TimeSyncAgeComp | 4824 | 6072 | 3168 | 4696 |
| M4_TimeSyncStepGuard | 4824 | 6072 | 3168 | 4696 |
| M4_TimeSyncShadow | 13920 | 19312 | 20464 | 14008 |

## Observations
- Baseline and AgeComp/StepGuard remain close on E55/E58 in this sweep.
- Shadow variants are higher in these new video stressors in this run.
- Pareto jitter (E57) favors AgeComp/StepGuard vs baseline/Shadow here.
