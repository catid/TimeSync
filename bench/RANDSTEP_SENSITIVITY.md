# E53 Random-Step Sensitivity (10 seeds)

Run: `bench/run_randstep_sensitivity.sh benchmarks/run_randstep_sensitivity_20260203`

Scenario: E53_randstep_normal
Methods:
- M4_TimeSync
- M4_TimeSyncShadow:step
- M4_TimeSyncShadow:step_xor_rtt
- M4_TimeSyncStepGuard
- M4_TimeSyncDDAC:step

## Summary (median / std across seeds)

| method | p95_med_us | p95_std_us | step_rec_med_s | step_rec_std_s | rec3_med_s | rec3_std_s |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 1,136,720 | 456,565 | 43.50 | 12.09 | 45.50 | 12.09 |
| M4_TimeSyncShadow:step | 1,136,700 | 456,569 | 44.00 | 11.72 | 46.00 | 11.72 |
| M4_TimeSyncShadow:step_xor_rtt | 1,136,710 | 456,569 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | 1,136,710 | 456,569 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | 1,136,710 | 456,564 | 0.00 | 0.00 | 0.00 | 0.00 |

Notes:
- StepGuard/DDAC:step/Shadow:step_xor_rtt did not recover to the 1ms threshold (step recovery = -1 in CSV), so rec_* medians report 0.
- Baseline and Shadow:step recover in ~44–46s on average; high variance reflects random step timing.
