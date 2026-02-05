# E132_uplink_saturation_ack_compress: q01_near_reset variants (poll_time_err_p95 median, 100 seeds)

Baseline M4_TimeSyncQuantile:q01_near_reset median_p95_us: 32905.8

| Rank | Method | median_p95_us | Δ vs baseline | Ratio | Status |
|---:|---|---:|---:|---:|---|
| 1 | M4_TimeSyncQuantile:q01_near_reset_rttguard | 32275.2 | -630.5 | 0.981 | PASS |
| 2 | M4_TimeSyncQuantile:q01_near_reset_xor | 32474.2 | -431.5 | 0.987 | PASS |
| 3 | M4_TimeSyncQuantile:q01_near_reset | 32905.8 | +0.0 | 1.000 | PASS |
| 4 | M4_TimeSyncQuantile:q01_near_reset_q05 | 32905.8 | +0.0 | 1.000 | PASS |
| 5 | M4_TimeSyncQuantile:q01_near_reset_timegate | 32905.8 | +0.0 | 1.000 | PASS |
| 6 | M4_TimeSyncQuantile:q01_near_reset_cse | 32911.2 | +5.5 | 1.000 | FAIL |
| 7 | M4_TimeSyncQuantile:q01_near_reset_cse_blend | 32911.8 | +6.0 | 1.000 | FAIL |
| 8 | M4_TimeSyncQuantile:q01_near_reset_tilted | 32912.5 | +6.8 | 1.000 | FAIL |
| 9 | M4_TimeSyncQuantile:q01_near_reset_shrink | 33498.2 | +592.5 | 1.018 | FAIL |
| 10 | M4_TimeSyncQuantile:q01_near_reset_win500 | 39531.5 | +6625.8 | 1.201 | FAIL |
| 11 | M4_TimeSyncQuantile:q01_near_reset_flip_reset | 258407.8 | +225502.0 | 7.853 | FAIL |