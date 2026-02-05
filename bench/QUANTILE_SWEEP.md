# TimeSync Quantile Envelope Sweep

Run: benchmarks/run_peer_20260202_182929_quantile

| method | E0_noise_floor | E1_stationary_jitter | E4_loss_burst | E5_reorder_dup | E7_drift |
| --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 16 | 160 | 3448 |
| M4_TimeSyncQuantile:q01 | 0 | 32 | 32 | 280 | 3617 |
| M4_TimeSyncQuantile:q05 | 0 | 56 | 32 | 32 | 3586 |
| M4_TimeSyncQuantile:q10 | 8 | 72 | 72 | 32 | 4570 |

Notes:
- Quantile envelope did not improve E7 drift and regressed E1/E4/E5 vs baseline.
