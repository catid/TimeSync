# Hysteresis/CUSUM Sweep (E0/E1/E7) — updated

Run: `benchmarks/run_peer_20260202_073211_hyst_cusum_sweep2`

## Median poll-time p95 (us)

### E0_noise_floor

All variants: **0/0 us** (AB/BA), overhead 366 bps.

### E1_stationary_jitter

**Hysteresis**

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncHysteresis | 72 | 80 | 366 |
| M4_TimeSyncHysteresis:t100_c1 | 608 | 608 | 366 |
| M4_TimeSyncHysteresis:t100_c1_h200ms | 296 | 184 | 366 |
| M4_TimeSyncHysteresis:t100_c1_h500ms | 592 | 616 | 366 |

**CUSUM**

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncCUSUM | 440 | 296 | 366 |
| M4_TimeSyncCUSUM:k50_h300 | 560 | 512 | 366 |
| M4_TimeSyncCUSUM:k50_h300_h200ms | 80 | 80 | 366 |
| M4_TimeSyncCUSUM:k75_h750 | 192 | 224 | 366 |
| M4_TimeSyncCUSUM:k75_h750_h200ms | 80 | 80 | 366 |

### E7_drift

**Hysteresis**

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncHysteresis | 2283 | 2253 | 366 |
| M4_TimeSyncHysteresis:t100_c1 | 362 | 381 | 366 |
| M4_TimeSyncHysteresis:t100_c1_h200ms | 447 | 2261 | 366 |
| M4_TimeSyncHysteresis:t100_c1_h500ms | 402 | 2262 | 366 |

**CUSUM**

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncCUSUM | 420 | 2275 | 366 |
| M4_TimeSyncCUSUM:k50_h300 | 384 | 2275 | 366 |
| M4_TimeSyncCUSUM:k50_h300_h200ms | 466 | 2256 | 366 |
| M4_TimeSyncCUSUM:k75_h750 | 420 | 2275 | 366 |
| M4_TimeSyncCUSUM:k75_h750_h200ms | 466 | 2256 | 366 |

## Notes

- Hysteresis `t100_c1` still best for E7 but degrades E1; adding a 200ms hold improves E1 but reintroduces the E7 BA tail.
- CUSUM hold variants greatly improve E1 jitter but **do not** fix the E7 BA tail.
