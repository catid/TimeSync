# Top-contender sweep with video + steps

- Run: `benchmarks/run_top_video_step_20260203_095507`
- Script: `bench/run_top_contenders.sh` with SCENARIOS override (E0–E7 + steps + E46–E48)
- SEEDS=3

## Summary
- Baseline-safe contenders run: `M4_TimeSync`, `M4_TimeSyncShadow`, `M4_TimeSyncShadow:step`, `M4_TimeSyncAgeComp`, `M4_TimeSyncDDAC:step`, `M4_TimeSyncStepGuard`.
- Video scenarios (E46–E48) remain stable for baseline-safe methods in this short sweep.
- Step scenarios still include sentinel-scale values in the report for some variants, indicating recovery metric/reporting still needs refinement when using prefix method matches.

## Artifacts
- `benchmarks/run_top_video_step_20260203_095507/peer_bench.csv`
- `benchmarks/run_top_video_step_20260203_095507/method_compare.md`
