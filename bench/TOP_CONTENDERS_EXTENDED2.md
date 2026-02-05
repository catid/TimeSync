# Top contenders sweep (E51/E52 included)

- Run: `benchmarks/run_top_contenders_ext2_20260203_101132`
- Scenarios: E0–E7, steps E14–E21, low-rate E33, video E35–E36 + E46–E52
- Methods: M4_TimeSync, M4_TimeSyncShadow, M4_TimeSyncShadow:step, M4_TimeSyncAgeComp, M4_TimeSyncDDAC:step, M4_TimeSyncStepGuard

## Summary
- Baseline-safe contenders remain closely grouped on extended video scenarios including E51/E52 in this short sweep.
- No method in this contender set beats baseline on E7 while staying E0–E6 safe.

## Artifacts
- `benchmarks/run_top_contenders_ext2_20260203_101132/peer_bench.csv`
- `benchmarks/run_top_contenders_ext2_20260203_101132/method_compare.md`
