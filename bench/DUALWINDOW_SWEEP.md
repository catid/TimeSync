# DualWindow Short-Window Sweep (E0/E1/E7)

Run: `benchmarks/run_peer_20260202_062916_dualwindow_sweep`

## Median poll-time p95 (us)

### E0_noise_floor

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncDualWindow | 2533 | 2526 | 366 |
| M4_TimeSyncDualWindow:w50k | 2533 | 2526 | 366 |
| M4_TimeSyncDualWindow:w100k | 2533 | 2526 | 366 |
| M4_TimeSyncDualWindow:w500k | 2533 | 2526 | 366 |

### E1_stationary_jitter

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncDualWindow | 2278 | 2011 | 366 |
| M4_TimeSyncDualWindow:w50k | 2270 | 2021 | 366 |
| M4_TimeSyncDualWindow:w100k | 2275 | 2014 | 366 |
| M4_TimeSyncDualWindow:w500k | 2280 | 2008 | 366 |

### E7_drift

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncDualWindow | 5504 | 5178 | 366 |
| M4_TimeSyncDualWindow:w50k | 5496 | 5183 | 366 |
| M4_TimeSyncDualWindow:w100k | 5503 | 5179 | 366 |
| M4_TimeSyncDualWindow:w500k | 5504 | 5177 | 366 |

## Notes

- Short-window choices did not materially change outcomes.
- DualWindow remains significantly worse than baseline on E0/E1/E7 in these runs.
