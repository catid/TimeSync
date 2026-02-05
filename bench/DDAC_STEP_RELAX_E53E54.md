# DDAC Step Relax Sweep (E53/E54)

Run: `benchmarks/run_ddac_step_relax_20260203_e53e54`
Script: `bench/run_ddac_step_relax.sh` (SCENARIOS_OVERRIDE=E53/E54)

Methods:
- M4_TimeSync
- M4_TimeSyncDDAC
- M4_TimeSyncDDAC:step
- M4_TimeSyncDDAC:step_relax

## Summary
- `step_relax` matches `step` for E53/E54 in this sweep (no improvement vs baseline).
- E53/E54 p95 errors are identical across DDAC variants under current gating.

Next:
- If we want DDAC to move on E53, consider loosening skew confidence or enabling alternative step criteria rather than just relaxing stability gates.
