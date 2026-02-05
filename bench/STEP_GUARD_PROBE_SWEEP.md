# StepGuard Probe Sweep (E14/E15/E18/E21)

Run:
- `benchmarks/run_stepguard_probe_20260203_085858/peer_bench.csv`

Setup:
- `SEEDS=1 DURATION=40` with exact scenario/method matching
- Methods: `M4_TimeSync`, `M4_TimeSyncStepGuard`, `M4_TimeSyncStepGuard:probe`

Summary:
- StepGuard reduces large‑step errors from overflow/67s‑scale to ~15s in this sweep.
- Probe variant matches StepGuard on steps, with higher overhead (414 vs 366 bps).
