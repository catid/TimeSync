# Drift-Range Sweep: VarGate vs Adaptive

Run: benchmarks/run_peer_20260202_075352_drift_range

| scenario | method | p95_ab_us | p95_ba_us | p95_max_us | overhead_bps |
| --- | --- | --- | --- | --- | --- |
| E7_drift_100ppm | M4_TimeSyncAdaptive | 508 | 522 | 522 | 366 |
| E7_drift_100ppm | M4_TimeSyncAdaptive:t200 | 504 | 4368 | 4368 | 366 |
| E7_drift_100ppm | M4_TimeSyncAdaptive:t50 | 508 | 522 | 522 | 366 |
| E7_drift_100ppm | M4_TimeSyncVarGate | 508 | 522 | 522 | 366 |
| E7_drift_100ppm | M4_TimeSyncVarGate:hold200ms | 802 | 876 | 876 | 366 |
| E7_drift_100ppm | M4_TimeSyncVarGate:hold500ms | 600 | 648 | 648 | 366 |
| E7_drift_100ppm | M4_TimeSyncVarGate:t100_v20k_w20 | 508 | 522 | 522 | 366 |
| E7_drift_100ppm | M4_TimeSyncVarGate:t150_v50k_w40 | 516 | 522 | 522 | 366 |
| E7_drift_100ppm | M4_TimeSyncVarGate:t50_v5k_w10 | 508 | 522 | 522 | 366 |
| E7_drift_25ppm | M4_TimeSyncAdaptive | 312 | 333 | 333 | 366 |
| E7_drift_25ppm | M4_TimeSyncAdaptive:t200 | 260 | 1135 | 1135 | 366 |
| E7_drift_25ppm | M4_TimeSyncAdaptive:t50 | 304 | 333 | 333 | 366 |
| E7_drift_25ppm | M4_TimeSyncVarGate | 312 | 327 | 327 | 366 |
| E7_drift_25ppm | M4_TimeSyncVarGate:hold200ms | 1180 | 1122 | 1180 | 366 |
| E7_drift_25ppm | M4_TimeSyncVarGate:hold500ms | 1195 | 1129 | 1195 | 366 |
| E7_drift_25ppm | M4_TimeSyncVarGate:t100_v20k_w20 | 312 | 327 | 327 | 366 |
| E7_drift_25ppm | M4_TimeSyncVarGate:t150_v50k_w40 | 296 | 1133 | 1133 | 366 |
| E7_drift_25ppm | M4_TimeSyncVarGate:t50_v5k_w10 | 304 | 333 | 333 | 366 |
| E7_drift_200ppm | M4_TimeSync | 13600 | 13128 | 13600 | 366 |
| E7_drift_200ppm | M4_TimeSyncAdaptive | 828 | 848 | 848 | 366 |
| E7_drift_200ppm | M4_TimeSyncVarGate | 828 | 848 | 848 | 366 |
