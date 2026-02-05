# Step-focus sweep (top contenders)

- Run: `benchmarks/run_step_focus_20260203_094634`
- Script: `bench/run_step_only_multiseed.sh`
- Settings: SEEDS=3, DURATION=3, scenarios E14–E21 + E33/E39
- Methods: M4_TimeSync, M4_TimeSyncShadow:step, M4_TimeSyncShadow:step_xor_rtt, M4_TimeSyncStepGuard, M4_TimeSyncDDAC:step

## Notes
- `steps_compare.md` shows identical values across baseline/Shadow/StepGuard for this short-duration sweep; DDAC not listed in the table output (likely filtered or lacking recovery stats in report).
- Recovery table shows `-1.00` sentinels for all listed methods, indicating “no recovery within window” or report artifact for this short duration.

## Artifacts
- `benchmarks/run_step_focus_20260203_094634/peer_bench.csv`
- `benchmarks/run_step_focus_20260203_094634/steps_compare.md`
