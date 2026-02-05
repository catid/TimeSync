# Step recovery reporting fix

- Change: step_recover_s now returns -1 when recovery time <= step time (avoid 0s sentinel).
- Run: `benchmarks/run_step_only_fix_20260203_100810` (SEEDS=3, DURATION=6)

## Outcome
- Recovery table still shows 0.00s medians, suggesting recovery occurs within the same poll tick or recovery never crosses the 1ms threshold in these short runs.
- Next step: raise step threshold or log raw `step_recover_time_us` to distinguish fast recovery vs missing recovery.
