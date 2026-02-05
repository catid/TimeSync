# VarGate variants on E51_video_drift_congestion (poll_time_err_p95 median)

Baseline M4_TimeSyncVarGate: 37605.8 us

| Rank | Method | median_p95_us | Δ vs baseline | Ratio | Status |
|---:|---|---:|---:|---:|---|
| 1 | M4_TimeSyncVarGate:e51_hold500_robust | 36219.2 | -1386.5 | 0.963 | PASS |
| 2 | M4_TimeSyncVarGate:min500ms | 36278.5 | -1327.2 | 0.965 | PASS |
| 3 | M4_TimeSyncVarGate:e51_min500_robust | 36279.8 | -1326.0 | 0.965 | PASS |
| 4 | M4_TimeSyncVarGate:e51_hold500_v50k | 36289.0 | -1316.8 | 0.965 | PASS |
| 5 | M4_TimeSyncVarGate:e51_min500_t25_v2k | 36310.5 | -1295.2 | 0.966 | PASS |
| 6 | M4_TimeSyncVarGate:hold500ms | 36314.0 | -1291.8 | 0.966 | PASS |
| 7 | M4_TimeSyncVarGate:stationary | 36329.0 | -1276.8 | 0.966 | PASS |
| 8 | M4_TimeSyncVarGate:e51_stationary_hold500 | 36329.0 | -1276.8 | 0.966 | PASS |
| 9 | M4_TimeSyncVarGate:e51_stationary_t25_v2k | 36329.0 | -1276.8 | 0.966 | PASS |
| 10 | M4_TimeSyncVarGate:e51_hold500_t25_v2k | 36408.2 | -1197.5 | 0.968 | PASS |
| 11 | M4_TimeSyncVarGate:robust | 36431.2 | -1174.5 | 0.969 | PASS |
| 12 | M4_TimeSyncVarGate:e51_min500_hold200 | 36567.0 | -1038.8 | 0.972 | PASS |
| 13 | M4_TimeSyncVarGate:e51_min500_hold500 | 36706.5 | -899.2 | 0.976 | PASS |
| 14 | M4_TimeSyncVarGate:hold200ms | 37238.5 | -367.2 | 0.990 | PASS |
| 15 | M4_TimeSyncVarGate:t25_v2k_w10 | 37401.2 | -204.5 | 0.995 | PASS |
| 16 | M4_TimeSyncVarGate:e51_min500_v50k | 37537.0 | -68.8 | 0.998 | PASS |
| 17 | M4_TimeSyncVarGate | 37605.8 | +0.0 | 1.000 | FAIL |
| 18 | M4_TimeSyncVarGate:e51_v200k_w5_hold200 | 37701.5 | +95.8 | 1.003 | FAIL |
| 19 | M4_TimeSyncVarGate:t50_v5k_w10 | 37807.2 | +201.5 | 1.005 | FAIL |
| 20 | M4_TimeSyncVarGate:t100_v20k_w20 | 37923.5 | +317.8 | 1.008 | FAIL |
| 21 | M4_TimeSyncVarGate:t150_v50k_w40 | 38006.5 | +400.8 | 1.011 | FAIL |
| 22 | M4_TimeSyncVarGate:e51_robust_v200k | 38359.0 | +753.2 | 1.020 | FAIL |
| 23 | M4_TimeSyncVarGate:e51_v1m_w10 | 38466.2 | +860.5 | 1.023 | FAIL |
| 24 | M4_TimeSyncVarGate:e51_v500k_w10 | 38466.2 | +860.5 | 1.023 | FAIL |
| 25 | M4_TimeSyncVarGate:e51_t25_v200k_w10 | 38466.2 | +860.5 | 1.023 | FAIL |
| 26 | M4_TimeSyncVarGate:e51_v200k_w5 | 38466.2 | +860.5 | 1.023 | FAIL |
| 27 | M4_TimeSyncVarGate:e51_q01 | 38469.8 | +864.0 | 1.023 | FAIL |
| 28 | M4_TimeSyncVarGate:e51_q05 | 38469.8 | +864.0 | 1.023 | FAIL |
| 29 | M4_TimeSyncVarGate:t100_v200k_w20 | 38480.5 | +874.8 | 1.023 | FAIL |
| 30 | M4_TimeSyncVarGate:e51_win1s | 38516.8 | +911.0 | 1.024 | FAIL |
| 31 | M4_TimeSyncVarGate:e51_win500ms | 38516.8 | +911.0 | 1.024 | FAIL |
