# Adaptive Trigger Sweep (E0/E1/E7)

Run: benchmarks/run_peer_20260202_075003_adaptive_trigger_sweep

## E0_noise_floor

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncAdaptive | 0 | 0 | 366 |
| M4_TimeSyncAdaptive:t200 | 0 | 0 | 366 |
| M4_TimeSyncAdaptive:t50 | 0 | 0 | 366 |

## E1_stationary_jitter

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncAdaptive | 608 | 608 | 366 |
| M4_TimeSyncAdaptive:t200 | 608 | 608 | 366 |
| M4_TimeSyncAdaptive:t50 | 608 | 608 | 366 |

## E7_drift

| variant | p95 AB | p95 BA | overhead (bps) |
| --- | --- | --- | --- |
| M4_TimeSyncAdaptive | 362 | 381 | 366 |
| M4_TimeSyncAdaptive:t200 | 349 | 2275 | 366 |
| M4_TimeSyncAdaptive:t50 | 362 | 381 | 366 |
