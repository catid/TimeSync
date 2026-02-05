# TimeSync Hybrid Drift Selector Sweep

Run: benchmarks/run_peer_20260202_183959_hybrid_v4

| method | E0_noise_floor | E1_stationary_jitter | E4_loss_burst | E5_reorder_dup | E7_drift |
| --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 16 | 160 | 3448 |
| M4_TimeSyncHybrid | 0 | 24 | 16 | 160 | 3448 |
| M4_TimeSyncHybrid:t50_s5 | 0 | 16 | 16 | 160 | 3448 |
| M4_TimeSyncHybrid:t25_s5 | 0 | 16 | 16 | 160 | 3448 |
| M4_TimeSyncHybrid:robust | 0 | 24 | 16 | 160 | 3448 |

Notes:
- Hybrid selector did not improve E7 (matches baseline); drift detection likely not triggering under current MinDelta exchange.
