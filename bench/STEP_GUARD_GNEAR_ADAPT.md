# StepGuard Adaptive GNEAR (IQR-based)

Change:
- Added `M4_TimeSyncStepGuard:gn_iqr` variant using IQR for GNEAR scaling
  (`step_gnear_use_iqr=true`, `step_gnear_k=0.5`).

Quick check run:
- `benchmarks/run_stepguard_gnear_20260203_083548/peer_bench.csv`
- Scenarios: `E28_noise_floor_quick`, `E27_clock_step_quick`

Result:
- No delta vs baseline in quick cases; overhead unchanged.
- Needs full step/loss/path scenarios for a real verdict.
