# StepReset vs Baseline (E0–E6)

Source: benchmarks/run_stepreset_e0e6_20260203_064927/peer_bench.csv

Baseline: `M4_TimeSync`  
Comparison: `M4_TimeSyncStepReset`

## Summary
- StepReset regresses multiple non-step scenarios (E1/E3/E4/E5/E6) relative to baseline, so it is not safe as a baseline+step-reset hybrid for E0–E6.

## Key Metrics (poll_time_err_p95, µs)

| scenario | M4_TimeSync | M4_TimeSyncStepReset | delta |
| --- | --- | --- | --- |
| E0_noise_floor | 0 | 0 | 0 |
| E1_stationary_jitter | 8 | 180 | +172 |
| E2_asymmetry_40ms | 20004 | 20028 | +24 |
| E3_bufferbloat | 0 | 20 | +20 |
| E4_loss_burst | 24 | 224 | +200 |
| E5_reorder_dup | 136 | 5032 | +4896 |
| E6_path_change | 8 | 124 | +116 |

## Verdict
StepReset fails the E0–E6 “no regression” requirement; it should not be enabled by default without stronger gating.
