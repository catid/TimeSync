# Shadow + CSE + StepGuard Sweep

Source: `benchmarks/run_shadow_cse_step_20260203_070857/peer_bench.csv`

## Summary
- `M4_TimeSyncShadowSkew:cse` and `cse_step` match baseline ShadowSkew behavior (no E7 or step improvement).
- Step recovery remains unchanged from baseline TimeSync; large steps still fail.

## Key Results (poll_time_err_p95, µs)

| method | E0 | E4 | E5 | E6 | E7 | E7_100ppm | E14 | E15 | E18 | within_baseline_E0_6 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 24 | 136 | 8 | 3480 | 6917 | 1000005 | 67108900 | 18446700000000000000 | yes |
| M4_TimeSyncShadowSkew | 0 | 24 | 40 | 8 | 3405 | 6584 | 1000005 | 15000000 | 15000000 | yes |
| M4_TimeSyncShadowSkew:cse | 0 | 24 | 40 | 8 | 3405 | 6584 | 1000005 | 15000000 | 15000000 | yes |
| M4_TimeSyncShadowSkew:cse_step | 0 | 24 | 40 | 8 | 3405 | 6584 | 1000005 | 15000000 | 15000000 | yes |

## Verdict
The CSE and StepGuard additions did not change outcomes for ShadowSkew in the current gating configuration.

## Update: E63–E70 (1s/120s steps)
Source: `benchmarks/run_shadow_cse_step_20260203_e63e70/peer_bench.csv`

Summary:
- `M4_TimeSyncShadowSkew`, `cse`, and `cse_step` remain identical on the new 1s/120s step set.
- 1s steps: ShadowSkew variants recover in ~1.1s on A‑fwd/B‑back, but ~8.1s on A‑back/B‑fwd; baseline TimeSync is ~7–10s.
- 120s steps: no recovery observed (0.00 entries in `steps_compare.md`) for all methods.
