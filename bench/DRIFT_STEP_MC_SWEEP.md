# Drift + Step Monte Carlo (E31)

Run:
- `benchmarks/run_drift_step_mc_20260203_085821/peer_bench.csv`
- Summary: `benchmarks/run_drift_step_mc_20260203_085821/drift_step_mc.md`

Notes:
- Random step on random side combined with drift ramp.
- Shadow reduces large‑step tails relative to baseline in this small sample.

## Skew random‑walk variant (E81)

Run:
- `benchmarks/run_drift_step_mc_20260203_e81/peer_bench.csv`
- Summary: `benchmarks/run_drift_step_mc_20260203_e81/drift_step_mc.md`

Summary (med / p95 poll_time_err_p95, µs):
- `M4_TimeSync`: 2447 / 2748
- `M4_TimeSyncShadow`: 2452 / 2711
- `M4_TimeSyncStepReset`: 765 / 948
