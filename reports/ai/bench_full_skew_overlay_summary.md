# Full Bench (skew overlay on all scenarios)

- Source: `benchmarks/run_peer_20260205_172349_full_skew_overlay`
- Rows: 4427700
- Methods: 751

## Top 10 (overall composite)
| rank | method | estimator | discipline | score | E0-6 | E7 | video | steps | baseline_safe |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_noguard_notilted | min | none | 0.965 | 786 | 0 | 5118 | 4870 | no |
| 2 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_notilted | min | none | 0.965 | 786 | 0 | 5118 | 4870 | no |
| 3 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max700_iqr100k_beta0p12_rttguard_s4u_win42s_notilted | min | none | 0.965 | 786 | 0 | 5118 | 4870 | no |
| 4 | M4_TimeSyncSkewCorrected:probe_skew_0p2hz | regress | none | 0.987 | 752 | 0 | 5976 | 5431 | no |
| 5 | M4_TimeSyncPerDirSkew:q1ppm_2B | regress | hybrid | 1.000 | 765 | 0 | 5949 | 5519 | no |
| 6 | M4_TimeSyncPerDirSkew:peer_only | regress | hybrid | 1.000 | 765 | 0 | 5949 | 5519 | no |
| 7 | M4_TimeSyncPerDirSkew:gate50 | regress | hybrid | 1.000 | 765 | 0 | 5949 | 5519 | no |
| 8 | M4_TimeSyncSloped:w10_gate | min | none | 1.000 | 765 | 0 | 5949 | 5519 | no |
| 9 | M4_TimeSyncSloped:w2s | min | none | 1.000 | 765 | 0 | 5949 | 5519 | yes |
| 10 | M4_TimeSyncPerDirSkew | regress | hybrid | 1.000 | 765 | 0 | 5949 | 5519 | no |

## Top 10 (baseline-safe)
| rank | method | estimator | discipline | score | E0-6 | E7 | video | steps |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | M4_TimeSyncSloped:w2s | min | none | 1.000 | 765 | 0 | 5949 | 5519 |
| 2 | M4_TimeSyncHybrid:t25_s5 | min | none | 1.000 | 765 | 0 | 5949 | 5519 |
| 3 | M4_TimeSyncHybrid:t50_s5 | min | none | 1.000 | 765 | 0 | 5949 | 5519 |
| 4 | M4_TimeSync | min | none | 1.000 | 765 | 0 | 5949 | 5519 |
| 5 | M4_TimeSyncHybrid | min | none | 1.002 | 765 | 0 | 6006 | 5519 |
| 6 | M4_TimeSyncHybrid:robust | min | none | 1.002 | 765 | 0 | 6006 | 5519 |
| 7 | M4_TimeSyncQuantile:q05 | min | none | 1.073 | 276 | 0 | 20085 | 5018 |
| 8 | M4_TimeSyncQuantile:q10 | min | none | 1.084 | 232 | 0 | 21368 | 5074 |
| 9 | M4_TimeSyncQuantileNearHit:loose | min | none | 1.142 | 309 | 0 | 21288 | 5074 |
| 10 | M4_TimeSyncQuantileNearHit | min | none | 1.145 | 314 | 0 | 21288 | 5074 |

## Top 10 (skew scenarios)
| rank | method | estimator | discipline | median_p95_us |
| --- | --- | --- | --- | --- |
| 1 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max200 | min | none | 2076 |
| 2 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmax200 | min | none | 2102 |
| 3 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p08_win20s | min | none | 2148 |
| 4 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_win20s | min | none | 2171 |
| 5 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p20 | min | none | 2174 |
| 6 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_beta0p20 | min | none | 2174 |
| 7 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08_net150 | min | none | 2176 |
| 8 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_net200 | min | none | 2188 |
| 9 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08_net75 | min | none | 2194 |
| 10 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p12 | min | none | 2194 |

## Top 10 (video scenarios)
| rank | method | estimator | discipline | median_p95_us |
| --- | --- | --- | --- | --- |
| 1 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_win60s | min | none | 4217 |
| 2 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08_net150 | min | none | 4311 |
| 3 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p08_win40s | min | none | 4340 |
| 4 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max600_iqr100k_beta0p08_win40s | min | none | 4343 |
| 5 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_net200 | min | none | 4391 |
| 6 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p20 | min | none | 4394 |
| 7 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k | min | none | 4629 |
| 8 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin1_max800_iqr100k | min | none | 4629 |
| 9 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_max1000 | min | none | 4629 |
| 10 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max800_iqr100k_beta0p06 | min | none | 4633 |

## Top 10 (steps/clock/jump scenarios)
| rank | method | estimator | discipline | median_p95_us |
| --- | --- | --- | --- | --- |
| 1 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_skewmin2_max700_iqr100k_beta0p12_rttguard_s4u_win42s_notilted | min | none | 5431 |
| 2 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_noguard_notilted | min | none | 5431 |
| 3 | M4_TimeSyncPolicy:qshrink_only_switch5_rttguard_promo100_notilted | min | none | 5431 |
| 4 | M4_TimeSyncSloped:blend_w2w10 | min | none | 5948 |
| 5 | M4_TimeSyncSloped:w10s | min | none | 5948 |
| 6 | M4_TimeSyncPerDirSkew:q1ppm_2B | regress | hybrid | 5949 |
| 7 | M4_TimeSyncPerDirSkew:gate50 | regress | hybrid | 5949 |
| 8 | M4_TimeSync | min | none | 5949 |
| 9 | M4_TimeSyncSloped:w2s | min | none | 5949 |
| 10 | M4_TimeSyncPerDirSkew | regress | hybrid | 5949 |
