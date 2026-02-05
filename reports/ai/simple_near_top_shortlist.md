# Simple + Near-Top Shortlist

- Source: `benchmarks/run_peer_20260205_172349_full_skew_overlay/peer_bench.csv`
- Objective: simple methods that still rank near top on average across scenarios
- Simplicity filter: no probes, no protocol overhead delta, exclude multi-controller families (policy/adaptive/shadow/moe/ddac/etc.)
- Baseline overhead: 366.0 bps
- Best global avg p95: 7054.1 us

## Recommended (simple + within 5% of best global avg)
| method | est | disc | avg_p95_us | med_p95_us | global_rank | gap_to_best |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 7060.8 | 3674.0 | 2 | 0.09% |
| M4_TimeSyncSloped:w2s | min | none | 7060.8 | 3674.0 | 3 | 0.09% |
| M4_TimeSyncSloped:w10_gate | min | none | 7060.9 | 3674.0 | 4 | 0.10% |
| M4_TimeSyncSloped:w2to10 | min | none | 7060.9 | 3674.0 | 5 | 0.10% |
| M4_TimeSyncSloped:w2to10_shrink | min | none | 7060.9 | 3674.0 | 6 | 0.10% |
| M4_TimeSyncHybrid:t50_s5 | min | none | 7062.7 | 3721.0 | 12 | 0.12% |
| M4_TimeSyncHybrid:t25_s5 | min | none | 7062.8 | 3721.0 | 13 | 0.12% |
| M4_TimeSyncSloped | min | none | 7084.4 | 3681.5 | 14 | 0.43% |
| M4_TimeSyncSloped:blend_w2w10 | min | none | 7096.8 | 3680.0 | 18 | 0.61% |
| M4_TimeSyncSloped:w10s | min | none | 7096.8 | 3680.0 | 19 | 0.61% |
| M4_TimeSyncHybrid:robust | min | none | 7100.8 | 3764.0 | 20 | 0.66% |
| M4_TimeSyncHybrid | min | none | 7111.9 | 3737.5 | 21 | 0.82% |

## Best Simple Methods (top 15 by avg p95)
| method | est | disc | avg_p95_us | med_p95_us | global_rank | gap_to_best |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 7060.8 | 3674.0 | 2 | 0.09% |
| M4_TimeSyncSloped:w2s | min | none | 7060.8 | 3674.0 | 3 | 0.09% |
| M4_TimeSyncSloped:w10_gate | min | none | 7060.9 | 3674.0 | 4 | 0.10% |
| M4_TimeSyncSloped:w2to10 | min | none | 7060.9 | 3674.0 | 5 | 0.10% |
| M4_TimeSyncSloped:w2to10_shrink | min | none | 7060.9 | 3674.0 | 6 | 0.10% |
| M4_TimeSyncHybrid:t50_s5 | min | none | 7062.7 | 3721.0 | 12 | 0.12% |
| M4_TimeSyncHybrid:t25_s5 | min | none | 7062.8 | 3721.0 | 13 | 0.12% |
| M4_TimeSyncSloped | min | none | 7084.4 | 3681.5 | 14 | 0.43% |
| M4_TimeSyncSloped:blend_w2w10 | min | none | 7096.8 | 3680.0 | 18 | 0.61% |
| M4_TimeSyncSloped:w10s | min | none | 7096.8 | 3680.0 | 19 | 0.61% |
| M4_TimeSyncHybrid:robust | min | none | 7100.8 | 3764.0 | 20 | 0.66% |
| M4_TimeSyncHybrid | min | none | 7111.9 | 3737.5 | 21 | 0.82% |
| M4_TimeSyncEnsemble:w02 | min | none | 7831.4 | 5519.0 | 22 | 11.02% |
| M4_TimeSyncSkew:baseline_fll | median | fll | 7908.3 | 3758.0 | 23 | 12.11% |
| M4_TimeSyncSkewReg:theilsen | theilsen | hybrid | 8360.8 | 5475.5 | 24 | 18.52% |
