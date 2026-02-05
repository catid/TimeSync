# Composite Scorecard (Weighted)

Sources:
- `benchmarks/run_peer_full_20260203e/peer_bench.csv`
- `benchmarks/run_video_sweep_20260203_ext10/peer_bench.csv`
- `benchmarks/run_stepreset_guarded_20260203_e63e70/peer_bench.csv`

Baseline: `M4_TimeSync` (tolerance 0.0%)
Weights: E0-6=3, E7=2, video=1, steps=1
Required groups: E0-6, E7, video, steps

Baseline group medians (p95 us):
- E0-6: 16, E7: 3448, video: 5452, steps: 1000020

## Top 10 (baseline-safe)

| method | estimator | discipline | score | E0-6 | E7 | video | steps | overhead_bps | baseline_safe |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step_xor | min | none | 0.949 | 16 | 3470 | 3448 | 1001030 | 366 | yes |
| M4_TimeSyncShadow:xor | min | none | 0.949 | 16 | 3470 | 3448 | 1001030 | 366 | yes |
| M4_TimeSyncHybrid:t25_s5 | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSyncSloped:w2s | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSync | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSyncHybrid:t50_s5 | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSyncShadow:age10s | min | none | 1.144 | 16 | 4510 | 7576 | 1001030 | 366 | yes |
| M4_TimeSyncShadowSkew:a0.2 | min | none | 1.188 | 16 | 3470 | 12548 | 1001030 | 366 | yes |
| M4_TimeSyncShadowSkew:cse | min | none | 1.188 | 16 | 3470 | 12548 | 1001030 | 366 | yes |
| M4_TimeSyncShadow:age0 | min | none | 1.188 | 16 | 3470 | 12548 | 1001030 | 366 | yes |

## Top 10 (overall)

| method | estimator | discipline | score | E0-6 | E7 | video | steps | overhead_bps | baseline_safe |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSyncShadow:step_xor | min | none | 0.949 | 16 | 3470 | 3448 | 1001030 | 366 | yes |
| M4_TimeSyncShadow:xor | min | none | 0.949 | 16 | 3470 | 3448 | 1001030 | 366 | yes |
| M4_TimeSyncHybrid:t25_s5 | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSyncSloped:w2s | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSync | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSyncHybrid:t50_s5 | min | none | 1.000 | 16 | 3448 | 5452 | 1000020 | 366 | yes |
| M4_TimeSyncSloped | min | none | 1.134 | 21 | 3446 | 5450 | 1000080 | 366 | no |
| M4_TimeSyncSloped:w10s | min | none | 1.134 | 21 | 3446 | 5450 | 1000080 | 366 | no |
| M4_TimeSyncShadow:age10s | min | none | 1.144 | 16 | 4510 | 7576 | 1001030 | 366 | yes |
| M4_TimeSyncShadowSkew:a0.2 | min | none | 1.188 | 16 | 3470 | 12548 | 1001030 | 366 | yes |
