# Guarded StepReset Sweep

Source: `benchmarks/run_stepreset_guarded_20260203_065956/peer_bench.csv`

## Summary
- The `guard_xor` variant improves some small-step recovery vs baseline but still regresses E0–E6 (notably E1/E5/E6).
- Step recovery remains poor for large steps (E18/E19/E21), matching the original StepReset failure mode.

## E0–E6 Regression Check (poll_time_err_p95, µs)

| scenario | M4_TimeSync | M4_TimeSyncStepReset | M4_TimeSyncStepReset:guard_xor |
| --- | --- | --- | --- |
| E0_noise_floor | 0 | 0 | 0 |
| E1_stationary_jitter | 12 | 180 | 96 |
| E2_asymmetry_40ms | 20008 | 20028 | 20020 |
| E3_bufferbloat | 0 | 20 | 8 |
| E4_loss_burst | 24 | 224 | 44 |
| E5_reorder_dup | 136 | 5032 | 4908 |
| E6_path_change | 8 | 124 | 20 |

## Step Recovery (median seconds)

| method | recovery_med_s | E14 | E15 | E16 | E17 | E18 | E19 | E20 | E21 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 7.72 | 7.10 | 1.10 | 7.10 | 9.60 | 1.10 | 8.35 | 9.60 | 8.75 |
| M4_TimeSyncStepReset | 1.10 | 1.10 | 1.10 | 1.10 | 2.10 | 1.10 | 1.10 | 2.10 | 1.10 |
| M4_TimeSyncStepReset:guard_xor | 1.43 | 1.10 | 1.10 | 1.10 | 2.60 | 1.10 | 1.75 | 2.10 | 1.80 |

## Verdict
Guarding reduces some regressions but StepReset still violates E0–E6 requirements and does not fix large-step scenarios.

## Update: E63–E70 (1s/120s steps)

Source: `benchmarks/run_stepreset_guarded_20260203_e63e70/peer_bench.csv`

Notes:
- Added `M4_TimeSyncStepReset:sticky` (sticky-direction gating). Results match `guard_xor` on 1s steps.
- 1s steps (p95): guard_xor/sticky reduce A‑fwd/B‑fwd to ~24µs and A‑back/B‑back to ~48–88µs; baseline remains ~500ms.
- 120s steps remain huge for all StepReset variants (no recovery observed).
