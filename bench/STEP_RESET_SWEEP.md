# Step Reset / Step Guard Sweep (E0–E21)

Source: `benchmarks/run_stepguard_20260203_063828/peer_bench.csv`

Median p95 poll-time error (µs) across seeds; poll-valid rows only.

| method | E0 | E4 | E5 | E6 | E7 | E14 | E15 | E16 | E17 | E18 | E19 | E20 | E21 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 160 | 8 | 3448 | 1000010 | 67108900 | 1000010 | 1000020 | ≥1e12 | ≥1e12 | 1000000 | ≥1e12 |
| M4_TimeSyncStepGuard | 0 | 16 | 56 | 8 | 4620 | 1000010 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepGuard:th0p5ms | 0 | 16 | 56 | 8 | 4620 | 1000010 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepGuard:th2ms | 0 | 16 | 56 | 8 | 4620 | 1000010 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepReset | 0 | 104 | 5032 | 96 | 2253 | 88 | 67108900 | 88 | 224 | ≥1e12 | ≥1e12 | 200 | ≥1e12 |

Notes:
- StepGuard improves large-step handling to ~15s p95 but regresses E7 vs baseline.
- StepReset helps small steps (E14/E16/E20) but not large steps; E4/E5 regressions remain.
