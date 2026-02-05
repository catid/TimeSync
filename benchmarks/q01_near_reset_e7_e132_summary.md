# q01_near_reset variants vs baseline (poll_time_err_p95 median)

Baseline M4_TimeSyncQuantile:q01_near_reset: E7=8925.5 us, E132=32905.8 us

| Method | E7_median_p95 | ΔE7 | RatioE7 | E132_median_p95 | ΔE132 | RatioE132 | Status |
|---|---:|---:|---:|---:|---:|---:|---|
| M4_TimeSyncQuantile:q01_near_reset | 8925.5 | +0.0 | 1.000 | 32905.8 | +0.0 | 1.000 | PASS |
| M4_TimeSyncQuantile:q01_near_reset_cse | 8923.8 | -1.8 | 1.000 | 32911.2 | +5.5 | 1.000 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_cse_blend | 8925.8 | +0.2 | 1.000 | 32911.8 | +6.0 | 1.000 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_flip_reset | 244764.5 | +235839.0 | 27.423 | 258407.8 | +225502.0 | 7.853 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_q05 | 8925.5 | +0.0 | 1.000 | 32905.8 | +0.0 | 1.000 | PASS |
| M4_TimeSyncQuantile:q01_near_reset_rttguard | 9103.5 | +178.0 | 1.020 | 32275.2 | -630.5 | 0.981 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_shrink | 1797.0 | -7128.5 | 0.201 | 33498.2 | +592.5 | 1.018 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_tilted | 8924.8 | -0.8 | 1.000 | 32912.5 | +6.8 | 1.000 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_timegate | 8925.5 | +0.0 | 1.000 | 32905.8 | +0.0 | 1.000 | PASS |
| M4_TimeSyncQuantile:q01_near_reset_win500 | 835.2 | -8090.2 | 0.094 | 39531.5 | +6625.8 | 1.201 | FAIL |
| M4_TimeSyncQuantile:q01_near_reset_xor | 8916.2 | -9.2 | 0.999 | 32474.2 | -431.5 | 0.987 | PASS |