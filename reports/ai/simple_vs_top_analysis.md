# Simple + Strong Algorithm Analysis

- Source: `benchmarks/run_peer_20260205_172349_full_skew_overlay/peer_bench.csv`
- Metric: per-scenario median `poll_time_err_p95` (AB/BA max), compared to best method per scenario
- Baseline overhead (`M4_TimeSync`): 366.0 bps

Simplicity score = `active_modules + 2*(probe_used) + 2*(protocol_overhead_delta)`

## Recommended (simple and near-top)
| method | est | disc | simplicity | geom_ratio_to_best | mean_ratio_to_best | <=1% best (scenarios) | probe_hz | overhead_delta_bps | active_modules |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |

## Top Overall (for reference)
| method | est | disc | simplicity | geom_ratio_to_best | mean_ratio_to_best | <=1% best | probe_hz | overhead_delta_bps | active_modules |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p10 | min | none | 1 | 2.1423 | 4.7540 | 2/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p12 | min | none | 1 | 2.1446 | 4.7549 | 2/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p10 | min | none | 1 | 2.1575 | 4.7849 | 2/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p12 | min | none | 1 | 2.1582 | 4.7839 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin1_max600_iqr100k_beta0p08 | min | none | 1 | 2.1591 | 4.7748 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p08 | min | none | 1 | 2.1591 | 4.7748 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr120k_beta0p08 | min | none | 1 | 2.1591 | 4.7748 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr80k_beta0p08 | min | none | 1 | 2.1613 | 4.7779 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p08_win40s | min | none | 1 | 2.1670 | 4.7841 | 3/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08 | min | none | 1 | 2.1730 | 4.8043 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08_win30s | min | none | 1 | 2.1730 | 4.8043 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr120k_beta0p08 | min | none | 1 | 2.1730 | 4.8043 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr80k_beta0p08 | min | none | 1 | 2.1753 | 4.8075 | 1/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08_win40s | min | none | 1 | 2.1840 | 4.8261 | 3/59 | 0.00 | 0.0 | tilted |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p08_win20s | min | none | 1 | 2.1851 | 4.8154 | 3/59 | 0.00 | 0.0 | tilted |

## Simplest Candidates
| method | est | disc | simplicity | geom_ratio_to_best | mean_ratio_to_best | <=1% best |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0 | 2.6697 | 3.7481 | 2/59 |
| M4_TimeSyncSloped:w2s | min | none | 0 | 2.6697 | 3.7481 | 2/59 |
| M4_TimeSyncSloped:w10_gate | min | none | 0 | 2.6701 | 3.7492 | 2/59 |
| M4_TimeSyncSloped:w2to10 | min | none | 0 | 2.6701 | 3.7492 | 2/59 |
| M4_TimeSyncSloped:w2to10_shrink | min | none | 0 | 2.6701 | 3.7492 | 2/59 |
| M4_TimeSyncHybrid:t50_s5 | min | none | 0 | 2.6716 | 3.7495 | 2/59 |
| M4_TimeSyncHybrid:t25_s5 | min | none | 0 | 2.6717 | 3.7496 | 2/59 |
| M4_TimeSyncSloped | min | none | 0 | 2.6843 | 3.7620 | 2/59 |
| M4_TimeSyncHybrid:robust | min | none | 0 | 2.6886 | 3.7621 | 2/59 |
| M4_TimeSyncHybrid | min | none | 0 | 2.6916 | 3.7640 | 2/59 |
| M4_TimeSyncSloped:blend_w2w10 | min | none | 0 | 2.6923 | 3.7700 | 2/59 |
| M4_TimeSyncSloped:w10s | min | none | 0 | 2.6923 | 3.7700 | 2/59 |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_noguard_notilted | min | none | 0 | 3.1213 | 7.1137 | 3/59 |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_notilted | min | none | 0 | 3.1213 | 7.1137 | 3/59 |
| M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max700_iqr100k_beta0p12_rttguard_s4u_win42s_notilted | min | none | 0 | 3.1213 | 7.1137 | 3/59 |
| M4_TimeSyncSkew:baseline_fll | median | fll | 0 | 3.1375 | 4.4735 | 0/59 |
| M4_TimeSyncEnsemble:w02 | min | none | 0 | 3.3197 | 4.6214 | 0/59 |
| M4_TimeSyncQuantile:q01_near_reset_xor | min | none | 0 | 3.3705 | 5.9209 | 1/59 |
| M4_TimeSyncSkewReg:theilsen | theilsen | hybrid | 0 | 3.3752 | 5.0578 | 0/59 |
| M4_TimeSyncQuantile:q01_near_reset_q05 | min | none | 0 | 3.3928 | 5.9629 | 5/59 |