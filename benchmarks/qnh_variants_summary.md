# QuantileNearHit variants on drift/skew scenarios (poll_time_err_p95 median)

Baseline: M4_TimeSyncQuantileNearHit:hi0.05

## E114_skew_sign_flip
Baseline M4_TimeSyncQuantileNearHit:hi0.05: 863.2 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncQuantileNearHit | 854.0 | -9.2 | 0.989 |
| M4_TimeSyncQuantileNearHit:loose | 854.0 | -9.2 | 0.989 |
| M4_TimeSyncQuantileNearHit:qnh_high0p10 | 854.0 | -9.2 | 0.989 |
| M4_TimeSyncQuantileNearHit:qnh_cse | 860.0 | -3.2 | 0.996 |
| M4_TimeSyncQuantileNearHit:qnh_cse_blend | 861.8 | -1.5 | 0.998 |
| M4_TimeSyncQuantileNearHit:qnh_low0p01 | 863.0 | -0.2 | 1.000 |
| M4_TimeSyncQuantileNearHit:hi0.05 | 863.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_enter2_exit10 | 863.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_hold_short | 863.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_min100 | 863.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset | 863.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset_xor | 863.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_short1s | 863.2 | +0.0 | 1.000 |

## E7_drift_200ppm
Baseline M4_TimeSyncQuantileNearHit:hi0.05: 9123.0 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncQuantileNearHit | 9097.5 | -25.5 | 0.997 |
| M4_TimeSyncQuantileNearHit:loose | 9097.5 | -25.5 | 0.997 |
| M4_TimeSyncQuantileNearHit:qnh_high0p10 | 9097.5 | -25.5 | 0.997 |
| M4_TimeSyncQuantileNearHit:qnh_cse | 9122.5 | -0.5 | 1.000 |
| M4_TimeSyncQuantileNearHit:hi0.05 | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_enter2_exit10 | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_hold_short | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_low0p01 | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_min100 | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset_xor | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_short1s | 9123.0 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_cse_blend | 9123.5 | +0.5 | 1.000 |

## E7_drift_400ppm
Baseline M4_TimeSyncQuantileNearHit:hi0.05: 18350.5 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncQuantileNearHit | 18302.2 | -48.2 | 0.997 |
| M4_TimeSyncQuantileNearHit:loose | 18302.2 | -48.2 | 0.997 |
| M4_TimeSyncQuantileNearHit:qnh_high0p10 | 18302.2 | -48.2 | 0.997 |
| M4_TimeSyncQuantileNearHit:qnh_cse | 18343.8 | -6.8 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_cse_blend | 18346.8 | -3.8 | 1.000 |
| M4_TimeSyncQuantileNearHit:hi0.05 | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_enter2_exit10 | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_hold_short | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_low0p01 | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_min100 | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset_xor | 18350.5 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_short1s | 18350.5 | +0.0 | 1.000 |

## E133_temp_drift_sine_step
Baseline M4_TimeSyncQuantileNearHit:hi0.05: 2303.2 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncQuantileNearHit | 2285.8 | -17.5 | 0.992 |
| M4_TimeSyncQuantileNearHit:loose | 2285.8 | -17.5 | 0.992 |
| M4_TimeSyncQuantileNearHit:qnh_high0p10 | 2285.8 | -17.5 | 0.992 |
| M4_TimeSyncQuantileNearHit:qnh_hold_short | 2288.5 | -14.8 | 0.994 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset | 2288.5 | -14.8 | 0.994 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset_xor | 2300.5 | -2.8 | 0.999 |
| M4_TimeSyncQuantileNearHit:qnh_cse | 2302.5 | -0.8 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_cse_blend | 2302.5 | -0.8 | 1.000 |
| M4_TimeSyncQuantileNearHit:hi0.05 | 2303.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_enter2_exit10 | 2303.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_low0p01 | 2303.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_min100 | 2303.2 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_short1s | 2303.2 | +0.0 | 1.000 |

## E92_clock_slew
Baseline M4_TimeSyncQuantileNearHit:hi0.05: 1326.8 us

| Method | median_p95_us | Δ vs baseline | Ratio |
|---|---:|---:|---:|
| M4_TimeSyncQuantileNearHit | 1304.2 | -22.5 | 0.983 |
| M4_TimeSyncQuantileNearHit:loose | 1304.2 | -22.5 | 0.983 |
| M4_TimeSyncQuantileNearHit:qnh_high0p10 | 1304.2 | -22.5 | 0.983 |
| M4_TimeSyncQuantileNearHit:qnh_cse | 1325.5 | -1.2 | 0.999 |
| M4_TimeSyncQuantileNearHit:qnh_cse_blend | 1326.2 | -0.5 | 1.000 |
| M4_TimeSyncQuantileNearHit:hi0.05 | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_enter2_exit10 | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_hold_short | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_low0p01 | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_min100 | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_near_reset_xor | 1326.8 | +0.0 | 1.000 |
| M4_TimeSyncQuantileNearHit:qnh_short1s | 1326.8 | +0.0 | 1.000 |

