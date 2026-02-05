# StepGuard Low-Rate Probe Sweep

Runs:
- `benchmarks/run_stepguard_probe_20260203_090855` (probe + probe_lowrate)
- `benchmarks/run_stepguard_probe_20260203_091846` (probe_relax)
- `benchmarks/run_stepguard_probe_20260203_092421` (probe_only)

## Summary
- Low-rate step scenario (2 Hz send, 1 Hz poll) still shows no improvement from probe-based step detection.
- `probe_only` behaves the same as previous probe variants in this quick sweep.
- Additional logic changes are still needed for probe-only step recovery under sparse traffic.

## Artifacts
- Latest method compare: `benchmarks/run_stepguard_probe_20260203_092421/method_compare.md`
- Latest merged CSV: `benchmarks/run_stepguard_probe_20260203_092421/peer_bench.csv`
