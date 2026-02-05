# Step recovery sweep (poll rate variants)

- Latest run: `benchmarks/run_step_recovery_20260203d`
- Script: `bench/run_step_recovery_sweep.sh`
- Poll rates: 1, 5, 10 Hz
- Scenarios: E14–E21 + E45 + E53 + E63–E70
- Methods: M4_TimeSync, M4_TimeSyncShadow:step, M4_TimeSyncStepGuard, M4_TimeSyncDDAC:step

## Observations
- `steps_compare.md` now includes 20/50/100/500 ms recovery thresholds (single‑hit + 3‑consecutive).
- 1s steps (E63–E66): A‑fwd/B‑back recover in ~2s for Shadow:step/StepGuard/DDAC:step, while baseline is ~10–11s; A‑back/B‑fwd remain ~9–11s for baseline/Shadow:step and show 0s (no recovery) for StepGuard/DDAC:step.
- Huge 120s steps (E67–E70) remain effectively unrecovered (0s in `steps_compare.md`) across baseline‑safe methods.
- Large clock jumps (E45 large B) continue to show no recovery at any threshold.

## Follow-up
- Consider probe‑based step detection or alternative recovery thresholds to quantify partial recovery on large jumps.
- Evaluate whether step‑specific resets should be relaxed for huge‑step cases (E67–E70) without regressing E0–E6.
