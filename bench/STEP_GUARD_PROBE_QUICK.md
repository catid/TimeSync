# StepGuard Probe Quick Check (E27/E28)

Run:
- `benchmarks/run_stepguard_probe_20260203_083323/peer_bench.csv`
- Scenarios: `E28_noise_floor_quick`, `E27_clock_step_quick`
- Methods: `M4_TimeSync`, `M4_TimeSyncStepGuard`, `M4_TimeSyncStepGuard:probe`

Summary:
- Quick cases show identical error totals for all three methods (no visible improvement from probe offset on these short scenarios).
- Probe variant adds overhead (354 bps vs 306 bps) without changing quick-case errors.

Next:
- Use this quick run as a sanity check; still need full E14/E15/E18/E21 clock-step sweeps to judge probe benefit.
