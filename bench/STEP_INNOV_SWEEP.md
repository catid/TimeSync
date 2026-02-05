# Step Innovation Sweep

Source: `benchmarks/run_stepinnov_20260203_070235/peer_bench.csv`

## Summary
- `M4_TimeSyncStepGuard:mad8` does **not** recover from steps (recovery times -1 across E14–E21).
- It preserves E0–E6 baseline behavior, but step scenarios remain stuck at large errors.

## Step Recovery (median seconds)

| method | recovery_med_s | E14 | E15 | E16 | E17 | E18 | E19 | E20 | E21 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 7.72 | 7.10 | 1.10 | 7.10 | 9.60 | 1.10 | 8.35 | 9.60 | 8.75 |
| M4_TimeSyncStepGuard | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 |
| M4_TimeSyncStepGuard:mad8 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 | -1.00 |

## Verdict
Innovation-based MAD thresholding did not fix step recovery for the current StepGuard mechanism.
