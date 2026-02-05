# StepReset Holdoff Sweep

Run: `benchmarks/run_stepreset_holdoff_20260203_090526` (SEEDS=1, DURATION=40s, scenarios E6 + E14–E21).

## Summary
- Holdoff variants (`hold0p1s`, `hold1s`, `hold5s`, `hold10s`) show identical step recovery medians in this quick run.
- Guarded XOR variant is similar to baseline StepReset on step recovery, with slightly higher recovery on some large steps.
- No clear holdoff sensitivity observed with 40s duration + 1 seed; a longer sweep is needed to decide if holdoff affects post-step jitter or false resets.

## Artifacts
- Step recovery table: `benchmarks/run_stepreset_holdoff_20260203_090526/steps_compare.md`
- Full merged CSV: `benchmarks/run_stepreset_holdoff_20260203_090526/peer_bench.csv`
- Method compare (includes E6): `benchmarks/run_stepreset_holdoff_20260203_090526/method_compare.md`
