# Focused contenders sweep (E0–E7 + E59 + E61)

- Run: `benchmarks/run_focused_contenders_20260203e`
- Script: `bench/run_top_contenders.sh`
- Methods: `M4_TimeSync`, `M4_TimeSyncShadow`, `M4_TimeSyncShadow:step`, `M4_TimeSyncAgeComp`, `M4_TimeSyncDDAC:step`, `M4_TimeSyncStepGuard`
- Scenarios in sweep include E0–E7, E32–E37, E10–E13, E14–E21, E59, and E61 (table below highlights core + new E61).

## Key table (from peer_bench.csv)

| method | E0_noise_floor | E1_stationary_jitter | E2_asymmetry_40ms | E3_bufferbloat | E4_loss_burst | E5_reorder_dup | E6_path_change | E7_drift | E59_path_flap | E61_video_congestion_120ms |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 0 | 16 | 20008 | 0 | 16 | 160 | 8 | 3448 | 22856 | 22856 |
| M4_TimeSyncShadow | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 3470 | 20160 | 28312 |
| M4_TimeSyncShadow:step | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 3470 | 20160 | 28312 |
| M4_TimeSyncAgeComp | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 4620 | 7496 | 5360 |
| M4_TimeSyncDDAC:step | 0 | 8 | 20008 | 0 | 16 | 48 | 8 | 5244 | 7496 | 5360 |
| M4_TimeSyncStepGuard | 0 | 8 | 20008 | 0 | 16 | 56 | 8 | 4620 | 7496 | 5360 |

## Observations
- Baseline/Shadow remain strongest on E7; AgeComp/DDAC/StepGuard trade some E7 performance for lower errors on E59/E61.
- E61 (congestion to 120ms) favors AgeComp/DDAC/StepGuard over Shadow variants in this run.
