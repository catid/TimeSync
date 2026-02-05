# Shadow / VarGate Step Tuning Sweep (E0–E21)

Source: `benchmarks/run_shadow_vargate_steps_20260203_064442/peer_bench.csv`

Median p95 poll-time error (µs) across seeds; poll-valid rows only.

| method | E0 | E4 | E5 | E6 | E7 | E14 | E15 | E16 | E17 | E18 | E19 | E20 | E21 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 24 | 136 | 8 | 3480 | 1000005 | 67108900 | 1000010 | 1000020 | ≥1e12 | ≥1e12 | 1000005 | ≥1e12 |
| M4_TimeSyncShadow | 0 | 24 | 40 | 8 | 3405 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncShadow:fast2 | 0 | 24 | 40 | 8 | 3077 | 1000005 | 15000000 | 999996 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncShadow:fast2_w1s | 0 | 24 | 5000 | 8 | 2626 | 1000005 | 36 | 999984 | 1000020 | 56 | 15000100 | 1000000 | 15000000 |
| M4_TimeSyncShadow:xor | 0 | 24 | 40 | 8 | 3405 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncShadow:rtt | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncShadow:xor_rtt | 0 | 24 | 40 | 8 | 4592 | 1000005 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncVarGate | 0 | 656 | 5044 | 268 | 388 | 160 | 67109200 | 252 | 272 | ≥1e12 | ≥1e12 | 136 | ≥1e12 |
| M4_TimeSyncVarGate:hold200ms | 0 | 44 | 4988 | 20 | 2257 | 28 | 67108900 | 28 | 499712 | ≥1e12 | ≥1e12 | 44 | ≥1e12 |
| M4_TimeSyncVarGate:hold500ms | 0 | 44 | 5000 | 52 | 2262 | 44 | 67108900 | 40 | 499712 | ≥1e12 | ≥1e12 | 44 | ≥1e12 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 0 | 656 | 5036 | 268 | 388 | 164 | 67109200 | 252 | 272 | ≥1e12 | ≥1e12 | 248 | ≥1e12 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 0 | 656 | 5032 | 268 | 388 | 160 | 67109200 | 252 | 272 | ≥1e12 | ≥1e12 | 136 | ≥1e12 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 0 | 492 | 5064 | 184 | 1358 | 44 | 67108900 | 48 | 260 | ≥1e12 | ≥1e12 | 44 | ≥1e12 |
| M4_TimeSyncVarGate:robust | 0 | 500 | 5032 | 268 | 388 | 160 | 67109050 | 144 | 272 | ≥1e12 | ≥1e12 | 136 | ≥1e12 |
| M4_TimeSyncVarGate:t25_v2k_w10 | 0 | 640 | 5044 | 260 | 388 | 292 | 67109200 | 252 | 272 | ≥1e12 | ≥1e12 | 248 | ≥1e12 |

Notes:
- Shadow: `fast2` improves E7 modestly without changing step outcomes; `fast2_w1s` improves some step cases but regresses E5 (5000 µs) and is inconsistent across large steps.
- VarGate variants continue to regress E4/E5/E6 and do not fix large-step failures.
