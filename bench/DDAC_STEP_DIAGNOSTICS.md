# DD-AC Step Diagnostics (E14–E21)

Source: `benchmarks/run_ddac_step_diag_20260203_065641/peer_bench.csv`

## Summary
- DD-AC step detection never triggers (`ddac_step_*` counters all zero) despite very large gap statistics.
- Median gap_in/out p95 is ~67,109,150 µs, so the step condition is not failing due to gap size. It is likely blocked by stability or RTT-guard gating.

## Diagnostics (median across seeds/scenarios)

| method | gap_in_mean_us | gap_in_p95_us | gap_out_mean_us | gap_out_p95_us | step_resets | step_in_hits | step_out_hits | step_xor_hits | step_streak_max |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSyncDDAC | 32985950 | 67109150 | 32985950 | 67109150 | 0 | 0 | 0 | 0 | 0 |
| M4_TimeSyncDDAC:step | 32985950 | 67109150 | 32985950 | 67109150 | 0 | 0 | 0 | 0 | 0 |
| M4_TimeSync | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |

## Implication
Step gating (stable_in/out and/or RTT guard) is blocking DD-AC step resets in the step scenarios. Next work should focus on diagnosing which gate fails and whether the stability thresholds or guard logic should be relaxed only for step detection.
