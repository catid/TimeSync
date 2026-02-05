# DD-AC Step Holdoff Sweep (E63–E70)

- Run: `benchmarks/run_ddac_step_holdoff_20260203e63`
- Script: `bench/run_ddac_step_holdoff.sh` (SCENARIOS_OVERRIDE=E63–E70)
- Seeds: 3
- Methods: M4_TimeSync baseline + DDAC step/holdoff/clamp variants

## Observations
- 1s steps (E63–E66): DDAC step/holdoff variants track similarly to baseline; no clear improvement vs `M4_TimeSync` in p95 error.
- Huge 120s steps (E67–E70): all DDAC step variants remain effectively unrecovered (p95 remains extremely large), matching baseline failure.
- Clamp/holdoff changes (`step_hold*`, `step_clamp*`) do not materially improve recovery on these new scenarios.

## Notes
- Results derived from `benchmarks/run_ddac_step_holdoff_20260203e63/method_compare.md`.
- Consider exploring probe-assisted step resets or alternate thresholds for E67–E70.
