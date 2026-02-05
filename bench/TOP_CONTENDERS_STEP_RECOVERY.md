# Top contenders sweep (incl. E53/E54)

- Run: `benchmarks/run_top_contenders_20260203_102140`
- Script: `bench/run_top_contenders.sh`
- Scenarios: default top-contender list + `E53_randstep_normal`, `E54_drift_sign_flip`
- Methods: `M4_TimeSync`, `M4_TimeSyncShadow`, `M4_TimeSyncShadow:step`, `M4_TimeSyncAgeComp`, `M4_TimeSyncDDAC:step`, `M4_TimeSyncStepGuard`

## Key table (from method_compare.md)

| method | E0_noise_floor | E1_stationary_jitter | E2_asymmetry_40ms | E3_bufferbloat | E4_loss_burst | E5_reorder_dup | E6_path_change | E7_drift | E53_randstep_normal | E54_drift_sign_flip |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 20008 | 0 | 16 | 160 | 8 | 3448 | 837135 | 3440 |
| M4_TimeSyncShadow:step | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 3470 | 837143 | 3320 |
| M4_TimeSyncShadow | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 3470 | 837143 | 3320 |
| M4_TimeSyncAgeComp | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 4620 | 837143 | 4462 |
| M4_TimeSyncStepGuard | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 4620 | 837143 | 4462 |
| M4_TimeSyncDDAC:step | 0 | 8 | 20008 | 0 | 16 | 48 | 8 | 5244 | 837191 | 4552 |

## Observations
- E0–E6 remain effectively baseline for Shadow and StepGuard variants in this run.
- E7 drift remains best for baseline/Shadow; AgeComp/StepGuard/DDAC show higher drift error in this sweep.
- E53 random step remains large for all methods here, indicating step recovery is not fully reflected in this aggregated score.
- E54 drift sign flip slightly favors Shadow variants vs baseline in this sweep.
