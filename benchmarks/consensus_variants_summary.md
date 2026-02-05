# Consensus variants on weak scenarios (poll_time_err_p95 median)

Baseline: M4_TimeSyncConsensus:noguard

## E7_drift_200ppm
Baseline M4_TimeSyncConsensus:noguard: 9022.0 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncConsensus:cng_p10_bin1 | 8971.0 | -51.0 | 0.994 |
| M4_TimeSyncConsensus:cng_shortbuf16 | 8983.5 | -38.5 | 0.996 |
| M4_TimeSyncConsensus:cng_shortbuf32 | 9012.0 | -10.0 | 0.999 |
| M4_TimeSyncConsensus:cng_floor100 | 9022.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_floor50 | 9022.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_frac02_floor100 | 9022.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_frac05_floor50 | 9022.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:noguard | 9022.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_shortbase_bin1 | 9045.5 | +23.5 | 1.003 |
| M4_TimeSyncConsensus:cng_p01_bin1 | 9144.0 | +122.0 | 1.014 |
| M4_TimeSyncConsensus | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:bin1 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:bin5 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:cand20 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:cand5 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:cng_guard_tight | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:k128 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:k32 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:loose | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:p1 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:p10 | 16078.5 | +7056.5 | 1.782 |
| M4_TimeSyncConsensus:shortbase | 16078.5 | +7056.5 | 1.782 |

## E74_video_path_bloat
Baseline M4_TimeSyncConsensus:noguard: 25776.0 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncConsensus:cng_p10_bin1 | 25430.0 | -346.0 | 0.987 |
| M4_TimeSyncConsensus:cng_floor100 | 25776.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_floor50 | 25776.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_frac02_floor100 | 25776.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_frac05_floor50 | 25776.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:noguard | 25776.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_shortbuf32 | 25804.0 | +28.0 | 1.001 |
| M4_TimeSyncConsensus:cng_shortbase_bin1 | 25856.0 | +80.0 | 1.003 |
| M4_TimeSyncConsensus | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:bin1 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:bin5 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:cand20 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:cand5 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:cng_guard_tight | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:cng_shortbuf16 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:k128 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:k32 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:loose | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:p1 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:p10 | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:shortbase | 26194.0 | +418.0 | 1.016 |
| M4_TimeSyncConsensus:cng_p01_bin1 | 26302.0 | +526.0 | 1.020 |

## E11_video_congestion
Baseline M4_TimeSyncConsensus:noguard: 16606.5 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncConsensus:cng_p10_bin1 | 16550.5 | -56.0 | 0.997 |
| M4_TimeSyncConsensus:cng_floor100 | 16606.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_floor50 | 16606.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_frac02_floor100 | 16606.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_frac05_floor50 | 16606.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:noguard | 16606.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_shortbase_bin1 | 16654.5 | +48.0 | 1.003 |
| M4_TimeSyncConsensus:cng_shortbuf32 | 16948.0 | +341.5 | 1.021 |
| M4_TimeSyncConsensus:cng_p01_bin1 | 17009.5 | +403.0 | 1.024 |
| M4_TimeSyncConsensus | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:bin1 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:bin5 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:cand20 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:cand5 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:cng_guard_tight | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:cng_shortbuf16 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:k128 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:k32 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:loose | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:p1 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:p10 | 17107.5 | +501.0 | 1.030 |
| M4_TimeSyncConsensus:shortbase | 17107.5 | +501.0 | 1.030 |

## E76_video_queue_sawtooth
Baseline M4_TimeSyncConsensus:noguard: 42670.0 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncConsensus | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:bin1 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:bin5 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:cand20 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:cand5 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:cng_guard_tight | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:k128 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:k32 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:loose | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:p1 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:p10 | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:shortbase | 21188.5 | -21481.5 | 0.497 |
| M4_TimeSyncConsensus:cng_frac02_floor100 | 28214.0 | -14456.0 | 0.661 |
| M4_TimeSyncConsensus:cng_frac05_floor50 | 28300.5 | -14369.5 | 0.663 |
| M4_TimeSyncConsensus:cng_p01_bin1 | 41432.5 | -1237.5 | 0.971 |
| M4_TimeSyncConsensus:cng_floor100 | 42670.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_floor50 | 42670.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:noguard | 42670.0 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_shortbase_bin1 | 42985.0 | +315.0 | 1.007 |
| M4_TimeSyncConsensus:cng_p10_bin1 | 43412.0 | +742.0 | 1.017 |
| M4_TimeSyncConsensus:cng_shortbuf32 | 49951.5 | +7281.5 | 1.171 |
| M4_TimeSyncConsensus:cng_shortbuf16 | 53137.5 | +10467.5 | 1.245 |

## E132_uplink_saturation_ack_compress
Baseline M4_TimeSyncConsensus:noguard: 29726.5 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncConsensus:cng_frac02_floor100 | 11903.0 | -17823.5 | 0.400 |
| M4_TimeSyncConsensus:cng_frac05_floor50 | 12385.0 | -17341.5 | 0.417 |
| M4_TimeSyncConsensus | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:bin1 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:bin5 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:cand20 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:cand5 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:cng_guard_tight | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:k128 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:k32 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:loose | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:p1 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:p10 | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:shortbase | 15469.0 | -14257.5 | 0.520 |
| M4_TimeSyncConsensus:cng_p01_bin1 | 28637.0 | -1089.5 | 0.963 |
| M4_TimeSyncConsensus:cng_floor100 | 29726.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_floor50 | 29726.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:noguard | 29726.5 | +0.0 | 1.000 |
| M4_TimeSyncConsensus:cng_shortbase_bin1 | 29762.0 | +35.5 | 1.001 |
| M4_TimeSyncConsensus:cng_p10_bin1 | 30595.0 | +868.5 | 1.029 |
| M4_TimeSyncConsensus:cng_shortbuf32 | 31262.0 | +1535.5 | 1.052 |
| M4_TimeSyncConsensus:cng_shortbuf16 | 31824.5 | +2098.0 | 1.071 |

