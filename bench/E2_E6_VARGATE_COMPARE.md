# E2-E6 Comparison: TimeSync vs VarGate

Run: benchmarks/run_peer_20260202_074559_e2e6_vargate

| scenario | method | p95_ab_us | p95_ba_us | p95_max_us |
| --- | --- | --- | --- | --- |
| E2_asymmetry_40ms | M4_TimeSync | 20008 | 20008 | 20008 |
| E2_asymmetry_40ms | M4_TimeSyncVarGate | 20032 | 20032 | 20032 |
| E3_bufferbloat | M4_TimeSync | 0 | 0 | 0 |
| E3_bufferbloat | M4_TimeSyncVarGate | 16 | 16 | 16 |
| E4_loss_burst | M4_TimeSync | 16 | 16 | 16 |
| E4_loss_burst | M4_TimeSyncVarGate | 456 | 488 | 488 |
| E5_reorder_dup | M4_TimeSync | 160 | 152 | 160 |
| E5_reorder_dup | M4_TimeSyncVarGate | 4984 | 5032 | 5032 |
| E6_path_change | M4_TimeSync | 8 | 8 | 8 |
| E6_path_change | M4_TimeSyncVarGate | 232 | 240 | 240 |