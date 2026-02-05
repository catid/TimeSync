# Step-only sweep (long duration)

- Run: `benchmarks/run_step_only_long_20260203_095134`
- Settings: SEEDS=3, DURATION=15
- Script: `bench/run_step_only_multiseed.sh`

## Summary
- After sentinel filtering, recovery tables report 0.00s across methods; step p95 values are small (8–24us) in this run.
- DDAC:step shows slightly lower p95 values in the table vs baseline, but recovery metrics still zero, suggesting step recovery is fast relative to the 1 Hz resolution or reporting still needs refinement.

## Artifacts
- `benchmarks/run_step_only_long_20260203_095134/peer_bench.csv`
- `benchmarks/run_step_only_long_20260203_095134/steps_compare.md`
