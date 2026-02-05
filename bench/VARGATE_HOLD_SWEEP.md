# VarGate Hold Sweep (E0/E1/E7)

Run: benchmarks/run_peer_20260202_074759_vargate_hold_sweep

## E0_noise_floor

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncVarGate | 0 | 0 | 366 |
| M4_TimeSyncVarGate:hold200ms | 0 | 0 | 366 |
| M4_TimeSyncVarGate:hold500ms | 0 | 0 | 366 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 0 | 0 | 366 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 0 | 0 | 366 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 0 | 0 | 366 |

## E1_stationary_jitter

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncVarGate | 608 | 544 | 366 |
| M4_TimeSyncVarGate:hold200ms | 112 | 80 | 366 |
| M4_TimeSyncVarGate:hold500ms | 304 | 152 | 366 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 608 | 544 | 366 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 616 | 616 | 366 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 608 | 608 | 366 |

## E7_drift

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncVarGate | 362 | 392 | 366 |
| M4_TimeSyncVarGate:hold200ms | 2283 | 2253 | 366 |
| M4_TimeSyncVarGate:hold500ms | 2283 | 2253 | 366 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 362 | 392 | 366 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 2143 | 2228 | 366 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 362 | 392 | 366 |
