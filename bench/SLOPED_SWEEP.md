# TimeSync Sloped-Floor Sweep

Run: benchmarks/run_peer_20260202_184529_sloped

| method | E0_noise_floor | E1_stationary_jitter | E4_loss_burst | E5_reorder_dup | E7_drift |
| --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 16 | 160 | 3448 |
| M4_TimeSyncSloped | 7 | 21 | 21 | 163 | 3446 |
| M4_TimeSyncSloped:w2s | 0 | 16 | 16 | 160 | 3448 |
| M4_TimeSyncSloped:w10s | 7 | 21 | 27 | 164 | 3446 |

Notes:
- Sloped-floor (min-delta regression) did not improve E7 and slightly regressed E0/E1 in some windows.
