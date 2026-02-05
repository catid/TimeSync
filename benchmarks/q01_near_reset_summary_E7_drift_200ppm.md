# E7_drift_200ppm: q01_near_reset variants (poll_time_err_p95 median, 100 seeds)

Baseline M4_TimeSyncQuantile:q01_near_reset median_p95_us: 8925.5

| Rank | Method | median_p95_us | Δ vs baseline | Ratio | Status |
|---:|---|---:|---:|---:|---|
| 1 | M4_TimeSyncQuantile:q01_near_reset_win500 | 835.2 | -8090.2 | 0.094 | PASS |
| 2 | M4_TimeSyncQuantile:q01_near_reset_shrink | 1797.0 | -7128.5 | 0.201 | PASS |
| 3 | M4_TimeSyncQuantile:q01_near_reset_xor | 8916.2 | -9.2 | 0.999 | PASS |
| 4 | M4_TimeSyncQuantile:q01_near_reset_cse | 8923.8 | -1.8 | 1.000 | PASS |
| 5 | M4_TimeSyncQuantile:q01_near_reset_tilted | 8924.8 | -0.8 | 1.000 | PASS |
| 6 | M4_TimeSyncQuantile:q01_near_reset | 8925.5 | +0.0 | 1.000 | PASS |
| 7 | M4_TimeSyncQuantile:q01_near_reset_q05 | 8925.5 | +0.0 | 1.000 | PASS |
| 8 | M4_TimeSyncQuantile:q01_near_reset_timegate | 8925.5 | +0.0 | 1.000 | PASS |
| 9 | M4_TimeSyncQuantile:q01_near_reset_cse_blend | 8925.8 | +0.2 | 1.000 | FAIL |
| 10 | M4_TimeSyncQuantile:q01_near_reset_rttguard | 9103.5 | +178.0 | 1.020 | FAIL |
| 11 | M4_TimeSyncQuantile:q01_near_reset_flip_reset | 244764.5 | +235839.0 | 27.423 | FAIL |