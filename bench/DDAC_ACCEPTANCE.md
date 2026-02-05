# DDAC / Shadow / StepGuard acceptance check

- Run: `benchmarks/run_acceptance_20260203_ext`
- Scenarios: E0–E6 + E7_drift (plus E59/E60/E61 for sanity; acceptance logic still checks E0–E7 only)
- Methods: all `M4_TimeSync*` variants (filtered by substring), focus on baseline-safe set below

## Findings
- `M4_TimeSync` passes baseline acceptance at 0% tolerance.
- `M4_TimeSyncShadow` and `M4_TimeSyncStepGuard` still fail the E7 improvement threshold (E7 above baseline).
- `M4_TimeSyncDDAC:step` reports poll metrics but remains above baseline on E7.

## Artifacts
- `benchmarks/run_acceptance_20260203_ext/acceptance.md`
- `benchmarks/run_acceptance_20260203_ext/method_compare.md`
