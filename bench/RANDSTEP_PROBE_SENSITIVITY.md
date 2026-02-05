# E53 Random-Step Probe Sensitivity (10 seeds)

Run: `benchmarks/run_randstep_probe_20260203`
Script: `bench/run_randstep_probe_sensitivity.sh`

| method | p95_med_us | p95_std_us | rec1_med_s | rec5_med_s | rec10_med_s | rec1_std_s |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 1,136,720 | 456,565 | 44.00 | 44.00 | 44.00 | 12.26 |
| M4_TimeSyncShadow:step | 1,136,700 | 456,569 | 44.00 | 44.00 | 44.00 | 11.72 |
| M4_TimeSyncStepGuard | 1,136,710 | 456,569 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:probe | 1,136,710 | 456,570 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:probe_relax | 1,136,710 | 456,570 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:probe_only | 1,136,710 | 456,570 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:probe_hard | 1,136,710 | 456,570 | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:probe_innov | 1,136,710 | 456,570 | 0.00 | 0.00 | 0.00 | 0.00 |

Notes:
- StepGuard probe variants still do not reach 1/5/10 ms recovery thresholds for E53 (rec_* medians 0 due to -1 in CSV).
- Baseline/Shadow:step recover in ~44s on average (same as prior sensitivity run).
