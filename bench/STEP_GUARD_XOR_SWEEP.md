# StepGuard XOR/Symmetric Gating Sweep (E0–E21)

Source: `benchmarks/run_stepguard_20260203_064234/peer_bench.csv`

Median p95 poll-time error (µs) across seeds; poll-valid rows only.

| method | E0 | E4 | E5 | E6 | E7 | E14 | E15 | E16 | E17 | E18 | E19 | E20 | E21 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 24 | 136 | 8 | 3480 | 1000005 | 67108900 | 1000010 | 1000020 | ≥1e12 | ≥1e12 | 1000005 | ≥1e12 |
| M4_TimeSyncStepGuard | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepGuard:th0p5ms | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepGuard:th2ms | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepGuard:xor | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepGuard:sym | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncStepReset | 0 | 224 | 5032 | 124 | 2257 | 76 | 67108950 | 84 | 244 | ≥1e12 | ≥1e12 | 236 | ≥1e12 |

Notes:
- XOR/symmetric gating does not change StepGuard outcomes in this sweep (identical metrics).
- StepGuard still regresses E7 relative to baseline; large steps improve to ~15s p95.
