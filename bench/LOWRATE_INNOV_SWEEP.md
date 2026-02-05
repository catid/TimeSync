# Low-rate StepGuard probe_innov sweep

- Run: `benchmarks/run_lowrate_innov_20260203_094320`
- Command: `SEEDS=3 DURATION=3` with low-rate scenarios (E33/E39/E41/E42/E43/E44)
- Methods: M4_TimeSync, M4_TimeSyncStepGuard, probe/probe_* variants incl. probe_innov

## Summary
- Across all low-rate scenarios, probe variants match baseline error magnitudes within noise; no clear improvement vs M4_TimeSync or StepGuard.
- `within_baseline_E0_6` flips to **no** for probe variants due to higher overhead_bps (66 vs 18), not accuracy gains.
- E44_lowrate_probe_burst remains flat across methods (720 in this run), indicating probe logic isn’t moving the needle in low-rate bursts.

## Artifacts
- `benchmarks/run_lowrate_innov_20260203_094320/peer_bench.csv`
- `benchmarks/run_lowrate_innov_20260203_094320/method_compare.md`
