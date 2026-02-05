# Low-Rate Step Sweep (E33/E39/E41/E42)

Run: `benchmarks/run_stepguard_probe_20260203_093427` (SEEDS=1, DURATION=60s).

## Summary
- StepGuard probe variants (probe, probe_lowrate, probe_relax, probe_only, probe_hard) show no improvement across low‑rate step scenarios.
- Baseline M4_TimeSync remains the reference; StepGuard variants still lag under sparse traffic + steps.

## Artifacts
- Method compare: `benchmarks/run_stepguard_probe_20260203_093427/method_compare.md`
- Merged CSV: `benchmarks/run_stepguard_probe_20260203_093427/peer_bench.csv`
