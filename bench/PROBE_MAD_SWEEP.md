# Probe MAD threshold sweep

- Run: `benchmarks/run_probe_mad_20260203_094445`
- Script: `bench/run_probe_mad_sweep.sh`
- Settings: SEEDS=3, DURATION=3, low-rate scenarios E33/E39/E41/E42/E43/E44
- Methods: baseline + StepGuard + probe_innov + probe_mad4/6/10

## Summary
- MAD multipliers (4/6/10) for probe_innov produce identical error levels to probe_innov and baseline across these low-rate scenarios.
- No measurable accuracy improvement; higher overhead_bps (66) keeps `within_baseline_E0_6` false.

## Artifacts
- `benchmarks/run_probe_mad_20260203_094445/peer_bench.csv`
- `benchmarks/run_probe_mad_20260203_094445/method_compare.md`
