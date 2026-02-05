# Clock-Step Monte Carlo Sweep

Goal: sample random clock step times/magnitudes and evaluate recovery distributions.

## Scenario

`E26_clock_step_mc` (random step on B):
- Step time uniformly in [10s, 60s]
- Step magnitude uniformly in [-30s, +30s]
- Base delay 60ms

## Run

```
SEEDS=10 THREADS=$(nproc) bench/run_clock_step_mc.sh
```

Outputs:
- `benchmarks/run_clock_step_mc_20260203_073453/peer_bench.csv`
- `benchmarks/run_clock_step_mc_20260203_073453/clock_step_mc.md`

## Summary (from clock_step_mc.md)

| method | poll_p95_med_us | poll_p95_p95_us |
| --- | --- | --- |
| M4_TimeSync | 67,108,900 | 1.84467e19 (overflow) |
| M4_TimeSyncShadow | 9,475,740 | 13,570,000 |
| M4_TimeSyncShadow:fast2 | 3,974,740 | 11,920,200 |
| M4_TimeSyncStepReset | 67,108,900 | 1.84467e19 (overflow) |

Notes:
- Shadow variants reduce large-step error from ~67s to ~4–10s in this MC sample.
- StepReset variants still overflow on large steps under randomized timing.
- step_recover_* fields were 0 in this run (no recovery flagged); poll_time_err_p95 remains the more reliable metric here.

## Random-side variant (E30)

See `bench/CLOCK_STEP_MC_SIDE_SWEEP.md` for random A/B side sampling using `E30_clock_step_mc_side`.

## Huge-step variant (E79)

`E79_clock_step_mc_huge` is available for ±120s random steps on a random side (see `tests/peer_bench.cpp`).
Use `SCENARIO_OVERRIDE=E79_clock_step_mc_huge` with `bench/run_clock_step_mc.sh`.

Run:
- `benchmarks/run_clock_step_mc_huge_20260203/peer_bench.csv`
- `benchmarks/run_clock_step_mc_huge_20260203/clock_step_mc.md`

Summary (med / p95 poll_time_err_p95, µs):
- `M4_TimeSync`: 105,927,000 / overflow
- `M4_TimeSyncShadow`: 67,108,900 / 134,218,000
- `M4_TimeSyncStepReset`: 67,108,900 / overflow
- `M4_TimeSyncProbe` (non‑baseline): 6,953,990 / 14,984,700

Notes:
- Huge random steps remain unrecovered for baseline‑safe methods; many variants still overflow on p95.
- Probe‑style methods reduce median error but are not E0–E6 compliant.
