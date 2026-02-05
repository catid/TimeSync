# StepGuard Low-Rate Sweeps (E33 + E39)

Runs:
- `benchmarks/run_stepguard_probe_20260203_092558` (probe_only)
- `benchmarks/run_stepguard_probe_20260203_093240` (adds probe_hard)

## Summary
- Probe variants including `probe_hard` do not improve E33/E39 low-rate step recovery in this quick sweep.
- Baseline M4_TimeSync remains unchanged; StepGuard variants still lag due to insufficient low-rate evidence.

## Artifacts
- Latest method compare: `benchmarks/run_stepguard_probe_20260203_093240/method_compare.md`
- Latest merged CSV: `benchmarks/run_stepguard_probe_20260203_093240/peer_bench.csv`
