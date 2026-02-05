# Composite Scorecard (Weighted)

Sources:
- `benchmarks/run_step_recovery_large_20260203_132838/peer_bench.csv`

Baseline: `M4_TimeSync` (tolerance 0.0%)
Weights: E0-6=3, E7=2, video=1, steps=1
Required groups: E0-6, E7, video, steps

Baseline group medians (p95 us):
- E0-6: 0, E7: 0, video: 0, steps: 134218000

## Top 10 (baseline-safe)

| method | estimator | discipline | score | E0-6 | E7 | video | steps | overhead_bps | baseline_safe |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |

## Top 10 (overall)

| method | estimator | discipline | score | E0-6 | E7 | video | steps | overhead_bps | baseline_safe |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
