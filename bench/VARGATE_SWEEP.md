# TimeSync VarGate Sweep (E0/E1/E7)

Run: `benchmarks/run_peer_20260202_062247_vargate_sweep`

## Median poll-time p95 (us)

### E0_noise_floor

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncVarGate | 0 | 0 | 366 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 0 | 0 | 366 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 0 | 0 | 366 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 0 | 0 | 366 |

### E1_stationary_jitter

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncVarGate | 608 | 544 | 366 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 608 | 608 | 366 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 608 | 544 | 366 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 616 | 616 | 366 |

### E7_drift

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncVarGate | 362 | 392 | 366 |
| M4_TimeSyncVarGate:t50_v5k_w10 | 362 | 392 | 366 |
| M4_TimeSyncVarGate:t100_v20k_w20 | 362 | 392 | 366 |
| M4_TimeSyncVarGate:t150_v50k_w40 | 2143 | 2228 | 366 |

## Notes

- `t150_v50k_w40` regresses on E7 drift; the other three variants are essentially identical to the default.
- No change in noise floor (E0) or stationary jitter (E1) for the tested variants.
