# Step-only benchmark report

- Source run: `benchmarks/run_step_only_20260203_093715/steps_compare.md`
- Scenarios: E14–E21 step variants + low-rate step (E33/E39) in the run

## Key observations
- StepGuard and Shadow:step_xor_rtt show the lowest `recovery_med_s` (0.00) in this report, while baseline M4_TimeSync has ~7–10s median recovery on small steps.
- Some columns show sentinel values (`-1.00` recovery seconds) and extremely large error values (`18446700000000000000`) that likely represent “no recovery within window” or overflow—needs handling in analysis code.
- Shadow:step and DDAC:step show higher median recovery times (8–62s) and less consistent step handling than StepGuard in this run.

## Follow-ups
- Sentinel/overflow handling added in `bench/report_steps_compare.py`; re-run step sweeps to refresh tables.
- Validate if `recovery_med_s=0.00` for StepGuard reflects immediate recovery or a reporting artifact.
