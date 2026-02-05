# GSP Holdoff (Age Gate) Sweep

Run: `benchmarks/run_gsp_holdoff_20260203_090702` (SEEDS=1, DURATION=40s).

## Summary
- Age gate variants (`age0`, `age1s`, `age5s`, `age10s`) show near-identical performance in this quick sweep.
- `fast2` (shorter consecutive requirement + shorter age gate) behaves similarly on E0–E6 and E7 drift with these parameters.
- No clear advantage of larger age gate in E7 drift; results suggest holdoff tuning needs a longer multi-seed sweep to see stability differences.

## Artifacts
- Method compare: `benchmarks/run_gsp_holdoff_20260203_090702/method_compare.md`
- Merged CSV: `benchmarks/run_gsp_holdoff_20260203_090702/peer_bench.csv`
