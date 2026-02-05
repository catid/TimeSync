# MoE Gating Sweep (E7/E53)

Run: `benchmarks/run_moe_e7_e53_20260203`
Script: `bench/run_moe_e7_e53.sh`

| method | E7_drift | E53_randstep_normal |
| --- | --- | --- |
| M4_TimeSync | 3448 | 837135 |
| M4_TimeSyncMoE | 4620 | 837143 |
| M4_TimeSyncMoE:guard1ms | 4620 | 837143 |
| M4_TimeSyncMoE:strict | 4620 | 837143 |
| M4_TimeSyncMoE:noguard | 489908 | 34724200 |
| M4_TimeSyncMoE:noguard_xor | 4604 | 34432400 |

## Summary
- MoE variants do not improve E7 or E53 vs baseline; strict/guarded variants are worse on E7.
- Unguarded MoE variants regress dramatically on E53.
