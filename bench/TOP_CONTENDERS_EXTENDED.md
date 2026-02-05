# Top contenders sweep (extended video + steps)

- Run: `benchmarks/run_top_contenders_ext_20260203_100605`
- Scenarios: E0–E7, steps E14–E21, low-rate E33, video E35–E36 + E46–E50
- Methods: M4_TimeSync, M4_TimeSyncShadow, M4_TimeSyncShadow:step, M4_TimeSyncAgeComp, M4_TimeSyncDDAC:step, M4_TimeSyncStepGuard

## Summary
- Baseline-safe contenders continue to track together on extended video scenarios (E46–E50) in this short sweep.
- DDAC:step now reports poll metrics in acceptance runs (see `bench/DDAC_ACCEPTANCE.md`) but still doesn’t beat baseline on E7 in this sweep.

## Artifacts
- `benchmarks/run_top_contenders_ext_20260203_100605/peer_bench.csv`
- `benchmarks/run_top_contenders_ext_20260203_100605/method_compare.md`
