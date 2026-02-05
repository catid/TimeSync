# Per-Direction Skew Exchange Sweep (E0/E1/E7)

Run: `benchmarks/run_peer_20260202_073507_perdir_sweep`

## Median poll-time p95 (us)

### E0_noise_floor

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncPerDirSkew | 0 | 0 | 382 |
| M4_TimeSyncPerDirSkew:peer_only | 0 | 0 | 382 |
| M4_TimeSyncPerDirSkew:gate50 | 0 | 0 | 382 |
| M4_TimeSyncPerDirSkew:q0.1ppm_4B | 0 | 0 | 374 |
| M4_TimeSyncPerDirSkew:q1ppm_2B | 0 | 0 | 370 |

### E1_stationary_jitter

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncPerDirSkew | 8 | 16 | 382 |
| M4_TimeSyncPerDirSkew:peer_only | 8 | 16 | 382 |
| M4_TimeSyncPerDirSkew:gate50 | 8 | 16 | 382 |
| M4_TimeSyncPerDirSkew:q0.1ppm_4B | 8 | 16 | 374 |
| M4_TimeSyncPerDirSkew:q1ppm_2B | 8 | 16 | 370 |

### E7_drift

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncPerDirSkew | 3458 | 3300 | 382 |
| M4_TimeSyncPerDirSkew:peer_only | 3458 | 3300 | 382 |
| M4_TimeSyncPerDirSkew:gate50 | 3458 | 3300 | 382 |
| M4_TimeSyncPerDirSkew:q0.1ppm_4B | 3458 | 3300 | 374 |
| M4_TimeSyncPerDirSkew:q1ppm_2B | 3458 | 3300 | 370 |

## Notes

- Sign-corrected peer skew + blend/gate/quantization does **not** improve E7 vs baseline.
- Quantized payloads reduce overhead but do not change accuracy in these scenarios.
