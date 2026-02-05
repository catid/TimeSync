# Clock jump benchmark (E45 variants)

- Run: `benchmarks/run_clock_jump_20260203c`
- Script: `bench/run_top_contenders.sh` with E45 scenarios
- Methods: `M4_TimeSync`, `M4_TimeSyncShadow`, `M4_TimeSyncShadow:step`, `M4_TimeSyncAgeComp`, `M4_TimeSyncDDAC:step`, `M4_TimeSyncStepGuard`

## Key table (from method_compare.md)

| method | E45_clock_jump_small_a_fwd | E45_clock_jump_small_a_back | E45_clock_jump_large_b_fwd | E45_clock_jump_large_b_back |
| --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step | 1000000 | 1000020 | 134218000 | 32889100 |
| M4_TimeSync | 1000020 | 1000010 | 134218000 | 0 |
| M4_TimeSyncDDAC:step | 1000000 | 1000020 | 134218000 | 47798500 |
| M4_TimeSyncAgeComp | 1000000 | 1000020 | 134218000 | 47698500 |
| M4_TimeSyncStepGuard | 1000000 | 1000020 | 127109000 | 47698500 |
| M4_TimeSyncShadow | 1000000 | 1000020 | 134218000 | 32889100 |

## Observations
- Small jumps (±2s on A) remain at ~1,000,000µs in this aggregate table for all contenders.
- Large B-forward jump dominates the error for most methods in this sweep; StepGuard is slightly lower but still high.
- Large B-backward jump shows variation across contenders (Shadow variants lower than AgeComp/DDAC/StepGuard in this run).
- Results are consistent with the previous clock-jump sweep; no method materially improves large‑jump recovery.

## Per‑poll‑rate sweep (poll1/5/10 Hz)

Run: `benchmarks/run_clock_jump_poll_20260203`
Note: per‑poll‑rate sweep not re-run in the 20260203c refresh.

Poll 1 Hz:

| method | E45_clock_jump_small_a_fwd | E45_clock_jump_small_a_back | E45_clock_jump_large_b_fwd | E45_clock_jump_large_b_back |
| --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step | 1000000 | 1000020 | 134218000 | 33289100 |
| M4_TimeSyncStepGuard | 1000000 | 1000020 | 127109000 | 48498500 |
| M4_TimeSyncDDAC:step | 1000000 | 1000020 | 134218000 | 48498500 |
| M4_TimeSync | 1000020 | 1000020 | 134218000 | 0 |

Poll 5 Hz:

| method | E45_clock_jump_small_a_fwd | E45_clock_jump_small_a_back | E45_clock_jump_large_b_fwd | E45_clock_jump_large_b_back |
| --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step | 1000000 | 1000020 | 134218000 | 32889100 |
| M4_TimeSyncStepGuard | 1000000 | 1000020 | 127109000 | 47698500 |
| M4_TimeSyncDDAC:step | 1000000 | 1000020 | 134218000 | 47898500 |
| M4_TimeSync | 1000020 | 1000010 | 134218000 | 0 |

Poll 10 Hz:

| method | E45_clock_jump_small_a_fwd | E45_clock_jump_small_a_back | E45_clock_jump_large_b_fwd | E45_clock_jump_large_b_back |
| --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step | 1000000 | 1000020 | 134218000 | 32889100 |
| M4_TimeSyncStepGuard | 1000000 | 1000020 | 127109000 | 47698500 |
| M4_TimeSyncDDAC:step | 1000000 | 1000020 | 134218000 | 47798500 |
| M4_TimeSync | 1000020 | 1000010 | 134218000 | 0 |
