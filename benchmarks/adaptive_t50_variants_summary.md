# Adaptive variants on video congestion scenarios (poll_time_err_p95 median)

Baseline: M4_TimeSyncAdaptive:t50

## E74_video_path_bloat
Baseline M4_TimeSyncAdaptive:t50: 34828.2 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncAdaptiveGuard:s5 | 1183.5 | -33644.8 | 0.034 |
| M4_TimeSyncAdaptiveGuard | 1370.8 | -33457.5 | 0.039 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_jitk3 | 17332.8 | -17495.5 | 0.498 |
| M4_TimeSyncAdaptiveBins | 26194.0 | -8634.2 | 0.752 |
| M4_TimeSyncAdaptiveBins:fast2 | 26194.0 | -8634.2 | 0.752 |
| M4_TimeSyncAdaptiveBins:rtt | 26194.0 | -8634.2 | 0.752 |
| M4_TimeSyncAdaptiveBins:xor | 26194.0 | -8634.2 | 0.752 |
| M4_TimeSyncAdaptive | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_age | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cnt3 | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cse | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_grow1p5 | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_gsp | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_jitter | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_combo | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_strict | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_shrink0p5 | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_stepguard | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:robust | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:stationary | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t200 | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t50 | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t500 | 34828.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:qswap | 34851.8 | +23.5 | 1.001 |
| M4_TimeSyncAdaptive:count3 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:grow1p25 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:gsp | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:jitterk3 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:min500ms | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rtt500 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr1000 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr600 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr700 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr800 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr900 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_combo | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_grow1p25 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_gsp | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short1s | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short4s | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_shrink0p5 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_stepguard | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_combo | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_grow1p25 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_gsp | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_jitk3 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_short1s | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_short4s | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_shrink0p5 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:rttiqr_stepguard | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:shrink0p5 | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:skewage | 36959.0 | +2130.8 | 1.061 |
| M4_TimeSyncAdaptive:stepguard | 36959.0 | +2130.8 | 1.061 |

## E11_video_congestion
Baseline M4_TimeSyncAdaptive:t50: 19867.5 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncAdaptiveGuard:s5 | 961.5 | -18906.0 | 0.048 |
| M4_TimeSyncAdaptiveGuard | 1078.8 | -18788.8 | 0.054 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_jitk3 | 1628.0 | -18239.5 | 0.082 |
| M4_TimeSyncAdaptiveBins | 17107.5 | -2760.0 | 0.861 |
| M4_TimeSyncAdaptiveBins:fast2 | 17107.5 | -2760.0 | 0.861 |
| M4_TimeSyncAdaptiveBins:rtt | 17107.5 | -2760.0 | 0.861 |
| M4_TimeSyncAdaptiveBins:xor | 17107.5 | -2760.0 | 0.861 |
| M4_TimeSyncAdaptive | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_age | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cnt3 | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cse | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_grow1p5 | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_gsp | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_jitter | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_combo | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_strict | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_shrink0p5 | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_stepguard | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:robust | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:stationary | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t200 | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t50 | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t500 | 19867.5 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:qswap | 21529.2 | +1661.8 | 1.084 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_shrink0p5 | 22653.0 | +2785.5 | 1.140 |
| M4_TimeSyncAdaptive:count3 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:grow1p25 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:gsp | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:jitterk3 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:min500ms | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rtt500 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr1000 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr600 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr700 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr800 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr900 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_combo | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_grow1p25 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_gsp | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short1s | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short4s | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_stepguard | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_combo | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_grow1p25 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_gsp | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_jitk3 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_short1s | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_short4s | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_shrink0p5 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:rttiqr_stepguard | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:shrink0p5 | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:skewage | 22661.5 | +2794.0 | 1.141 |
| M4_TimeSyncAdaptive:stepguard | 22661.5 | +2794.0 | 1.141 |

## E76_video_queue_sawtooth
Baseline M4_TimeSyncAdaptive:t50: 54269.2 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncAdaptiveBins:fast2 | 12670.0 | -41599.2 | 0.233 |
| M4_TimeSyncAdaptiveBins | 13214.0 | -41055.2 | 0.243 |
| M4_TimeSyncAdaptiveBins:xor | 13214.0 | -41055.2 | 0.243 |
| M4_TimeSyncAdaptiveBins:rtt | 21188.5 | -33080.8 | 0.390 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_jitk3 | 30096.8 | -24172.5 | 0.555 |
| M4_TimeSyncAdaptiveGuard | 30096.8 | -24172.5 | 0.555 |
| M4_TimeSyncAdaptiveGuard:s5 | 30096.8 | -24172.5 | 0.555 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_shrink0p5 | 30302.8 | -23966.5 | 0.558 |
| M4_TimeSyncAdaptive:count3 | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:rttiqr_cnt3 | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_combo | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_gsp | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short1s | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short4s | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_stepguard | 30357.2 | -23912.0 | 0.559 |
| M4_TimeSyncAdaptive:at50_cnt3 | 37666.5 | -16602.8 | 0.694 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_grow1p25 | 39989.8 | -14279.5 | 0.737 |
| M4_TimeSyncAdaptive:qswap | 49779.8 | -4489.5 | 0.917 |
| M4_TimeSyncAdaptive:jitterk3 | 51873.0 | -2396.2 | 0.956 |
| M4_TimeSyncAdaptive:rttiqr_jitk3 | 52069.5 | -2199.8 | 0.959 |
| M4_TimeSyncAdaptive:rttiqr_shrink0p5 | 52534.2 | -1735.0 | 0.968 |
| M4_TimeSyncAdaptive:shrink0p5 | 52534.2 | -1735.0 | 0.968 |
| M4_TimeSyncAdaptive:grow1p25 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:gsp | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:min500ms | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rtt500 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr1000 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr600 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr700 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr800 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr900 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr_combo | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr_grow1p25 | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr_gsp | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr_short1s | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr_short4s | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:rttiqr_stepguard | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:skewage | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:stepguard | 52552.0 | -1717.2 | 0.968 |
| M4_TimeSyncAdaptive:at50_jitter | 53628.5 | -640.8 | 0.988 |
| M4_TimeSyncAdaptive:at50_shrink0p5 | 54263.0 | -6.2 | 1.000 |
| M4_TimeSyncAdaptive:stationary | 54263.0 | -6.2 | 1.000 |
| M4_TimeSyncAdaptive:t200 | 54263.0 | -6.2 | 1.000 |
| M4_TimeSyncAdaptive:t500 | 54263.0 | -6.2 | 1.000 |
| M4_TimeSyncAdaptive | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_age | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cse | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_grow1p5 | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_gsp | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_combo | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_strict | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_stepguard | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:robust | 54269.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t50 | 54269.2 | +0.0 | 1.000 |

## E132_uplink_saturation_ack_compress
Baseline M4_TimeSyncAdaptive:t50: 40134.2 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncAdaptiveBins | 11142.0 | -28992.2 | 0.278 |
| M4_TimeSyncAdaptiveBins:xor | 12033.5 | -28100.8 | 0.300 |
| M4_TimeSyncAdaptiveBins:fast2 | 14211.0 | -25923.2 | 0.354 |
| M4_TimeSyncAdaptiveBins:rtt | 15469.0 | -24665.2 | 0.385 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_jitk3 | 33073.8 | -7060.5 | 0.824 |
| M4_TimeSyncAdaptiveGuard:s5 | 33073.8 | -7060.5 | 0.824 |
| M4_TimeSyncAdaptiveGuard | 33121.8 | -7012.5 | 0.825 |
| M4_TimeSyncAdaptive:count3 | 36277.2 | -3857.0 | 0.904 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short4s | 36593.2 | -3541.0 | 0.912 |
| M4_TimeSyncAdaptive:rttiqr_cnt3 | 36855.5 | -3278.8 | 0.918 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_combo | 36855.5 | -3278.8 | 0.918 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_gsp | 36855.5 | -3278.8 | 0.918 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_stepguard | 36855.5 | -3278.8 | 0.918 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_shrink0p5 | 36944.8 | -3189.5 | 0.921 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short1s | 37053.8 | -3080.5 | 0.923 |
| M4_TimeSyncAdaptive:rttiqr_jitk3 | 39704.2 | -430.0 | 0.989 |
| M4_TimeSyncAdaptive:grow1p25 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:gsp | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:min500ms | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rtt500 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr1000 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr600 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr700 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr800 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr900 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_combo | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_grow1p25 | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_gsp | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_short1s | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_short4s | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_stepguard | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:skewage | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:stepguard | 39722.8 | -411.5 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_shrink0p5 | 39729.2 | -405.0 | 0.990 |
| M4_TimeSyncAdaptive:shrink0p5 | 39729.2 | -405.0 | 0.990 |
| M4_TimeSyncAdaptive:qswap | 39749.5 | -384.8 | 0.990 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_grow1p25 | 39846.5 | -287.8 | 0.993 |
| M4_TimeSyncAdaptive:jitterk3 | 39902.2 | -232.0 | 0.994 |
| M4_TimeSyncAdaptive:at50_cnt3 | 39985.5 | -148.8 | 0.996 |
| M4_TimeSyncAdaptive:at50_jitter | 40125.0 | -9.2 | 1.000 |
| M4_TimeSyncAdaptive:at50_age | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cse | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_grow1p5 | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_gsp | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_combo | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_strict | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_stepguard | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t50 | 40134.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive | 40140.5 | +6.2 | 1.000 |
| M4_TimeSyncAdaptive:t200 | 40142.8 | +8.5 | 1.000 |
| M4_TimeSyncAdaptive:robust | 40146.8 | +12.5 | 1.000 |
| M4_TimeSyncAdaptive:stationary | 40152.0 | +17.8 | 1.000 |
| M4_TimeSyncAdaptive:t500 | 40152.0 | +17.8 | 1.000 |
| M4_TimeSyncAdaptive:at50_shrink0p5 | 40164.5 | +30.2 | 1.001 |

## E40_video_highlat_reorder
Baseline M4_TimeSyncAdaptive:t50: 32735.2 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncAdaptiveGuard:s5 | 11436.8 | -21298.5 | 0.349 |
| M4_TimeSyncAdaptiveGuard | 11481.2 | -21254.0 | 0.351 |
| M4_TimeSyncAdaptiveBins:fast2 | 11803.0 | -20932.2 | 0.361 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_jitk3 | 11888.2 | -20847.0 | 0.363 |
| M4_TimeSyncAdaptiveBins | 14591.5 | -18143.8 | 0.446 |
| M4_TimeSyncAdaptiveBins:xor | 14591.5 | -18143.8 | 0.446 |
| M4_TimeSyncAdaptiveBins:rtt | 18426.5 | -14308.8 | 0.563 |
| M4_TimeSyncAdaptive:count3 | 32441.5 | -293.8 | 0.991 |
| M4_TimeSyncAdaptive:qswap | 32524.0 | -211.2 | 0.994 |
| M4_TimeSyncAdaptive:at50_cnt3 | 32528.2 | -207.0 | 0.994 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short4s | 32533.8 | -201.5 | 0.994 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_shrink0p5 | 32552.2 | -183.0 | 0.994 |
| M4_TimeSyncAdaptive:rttiqr_cnt3 | 32600.5 | -134.8 | 0.996 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_combo | 32600.5 | -134.8 | 0.996 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_gsp | 32600.5 | -134.8 | 0.996 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_stepguard | 32600.5 | -134.8 | 0.996 |
| M4_TimeSyncAdaptive:at50_shrink0p5 | 32701.0 | -34.2 | 0.999 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_short1s | 32714.5 | -20.8 | 0.999 |
| M4_TimeSyncAdaptive:t500 | 32725.2 | -10.0 | 1.000 |
| M4_TimeSyncAdaptive | 32731.0 | -4.2 | 1.000 |
| M4_TimeSyncAdaptive:robust | 32731.0 | -4.2 | 1.000 |
| M4_TimeSyncAdaptive:stationary | 32731.0 | -4.2 | 1.000 |
| M4_TimeSyncAdaptive:t200 | 32731.0 | -4.2 | 1.000 |
| M4_TimeSyncAdaptive:at50_age | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_cse | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_grow1p5 | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_gsp | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_jitter | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_combo | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_rtt_strict | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:at50_stepguard | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:t50 | 32735.2 | +0.0 | 1.000 |
| M4_TimeSyncAdaptive:rttiqr_cnt3_grow1p25 | 32788.8 | +53.5 | 1.002 |
| M4_TimeSyncAdaptive:jitterk3 | 33405.0 | +669.8 | 1.020 |
| M4_TimeSyncAdaptive:grow1p25 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:gsp | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:min500ms | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rtt500 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr1000 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr600 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr700 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr800 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr900 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_combo | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_grow1p25 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_gsp | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_jitk3 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_short1s | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_short4s | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_shrink0p5 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:rttiqr_stepguard | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:shrink0p5 | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:skewage | 33474.5 | +739.2 | 1.023 |
| M4_TimeSyncAdaptive:stepguard | 33474.5 | +739.2 | 1.023 |

