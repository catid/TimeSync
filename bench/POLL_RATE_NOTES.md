# Poll-rate sweep notes

All sweeps use `bench/poll_rate_sweep.sh` with `DURATION=1`, `SEEDS=3`, `THREADS=3`,
and poll rates `2 5 10 20 50` Hz. Metrics report mean poll_time_err p95 (us) across seeds.

## E1_stationary_jitter

### M1 (Cristian baseline)

Source: `benchmarks/run_poll_rate_m1_20260131_001859/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 474.0 | 693.0 |
| 5 | 661.7 | 1050.3 |
| 10 | 687.0 | 1250.7 |
| 20 | 687.0 | 1250.7 |
| 50 | 687.0 | 1250.7 |

### M2 (NTP baseline)

Source: `benchmarks/run_poll_rate_20260131_000233/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 474.0 | 693.0 |
| 5 | 661.7 | 1050.3 |
| 10 | 687.0 | 1250.7 |
| 20 | 687.0 | 1250.7 |
| 50 | 687.0 | 1250.7 |

### M3 (PTP baseline)

Source: `benchmarks/run_poll_rate_m3_ptp_e1_20260131_002403/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 474.0 | 693.0 |
| 5 | 661.7 | 1050.3 |
| 10 | 687.0 | 1250.7 |
| 20 | 687.0 | 1250.7 |
| 50 | 687.0 | 1250.7 |

### M4 (TimeSync)

Source: `benchmarks/run_poll_rate_m4_timesync_20260131_001822/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 510133.3 | 489965.3 |
| 5 | 510232.0 | 489965.3 |
| 10 | 510522.7 | 489965.3 |
| 20 | 510522.7 | 489965.3 |
| 50 | 510362.7 | 489965.3 |

### M4 (Piggyback)

Source: `benchmarks/run_poll_rate_m4_piggyback_20260131_001841/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 511533.7 | 488964.0 |
| 5 | 511641.7 | 489096.3 |
| 10 | 511665.0 | 489112.2 |
| 20 | 511665.0 | 489112.2 |
| 50 | 511649.2 | 489107.0 |

### Observations (E1)

- M1, M2, and M3 track identically across poll rates for this scenario.
- For M1/M2/M3, 2 Hz shows lower mean p95 than 5-50 Hz; 10-50 Hz plateaus.
- For M4 (TimeSync/Piggyback), p95 poll-time error is ~0.49-0.51 s and mostly flat across rates.

## E7_drift

### M1 (Cristian baseline)

Source: `benchmarks/run_poll_rate_m1_e7_20260131_002527/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 162.3 | 247.0 |
| 5 | 230.7 | 367.0 |
| 10 | 272.0 | 370.3 |
| 20 | 273.7 | 370.3 |
| 50 | 270.7 | 369.0 |

### M2 (NTP baseline)

Source: `benchmarks/run_poll_rate_m2_e7_20260131_002543/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 162.3 | 247.0 |
| 5 | 230.7 | 367.0 |
| 10 | 272.0 | 370.3 |
| 20 | 273.7 | 370.3 |
| 50 | 270.7 | 369.0 |

### M3 (PTP baseline)

Source: `benchmarks/run_poll_rate_m3_ptp_e7_20260131_002425/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 162.3 | 247.0 |
| 5 | 230.7 | 367.0 |
| 10 | 272.0 | 370.3 |
| 20 | 273.7 | 370.3 |
| 50 | 270.7 | 369.0 |

### M4 (TimeSync)

Source: `benchmarks/run_poll_rate_m4_timesync_e7_20260131_002442/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 509977.7 | 489919.0 |
| 5 | 510008.7 | 489935.3 |
| 10 | 510077.7 | 489939.7 |
| 20 | 510077.7 | 489944.3 |
| 50 | 510051.7 | 489941.7 |

### M4 (Piggyback)

Source: `benchmarks/run_poll_rate_m4_piggyback_e7_20260131_002504/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 510344.7 | 489533.0 |
| 5 | 510377.8 | 489581.2 |
| 10 | 510391.5 | 489602.7 |
| 20 | 510394.3 | 489612.0 |
| 50 | 510386.3 | 489593.3 |

### Observations (E7)

- M1, M2, and M3 track identically across poll rates for this scenario.
- 2 Hz shows the lowest mean p95 for M1/M2/M3; 10-50 Hz is largely flat.
- M4 (TimeSync/Piggyback) p95 poll-time error stays around 0.49-0.51 s and is mostly flat across rates.

## E0_noise_floor

### M1 (Cristian baseline)

Source: `benchmarks/run_poll_rate_m1_e0_20260131_002826/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 14.0 | 14.3 |
| 5 | 22.0 | 14.3 |
| 10 | 25.0 | 14.7 |
| 20 | 25.0 | 14.7 |
| 50 | 22.3 | 14.3 |

### M4 (TimeSync)

Source: `benchmarks/run_poll_rate_m4_timesync_e0_20260131_002847/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 501000.0 | 499000.0 |
| 5 | 501002.7 | 499000.0 |
| 10 | 501005.3 | 499000.0 |
| 20 | 501005.3 | 499000.0 |
| 50 | 501005.3 | 499000.0 |

### M4 (Piggyback)

Source: `benchmarks/run_poll_rate_m4_piggyback_e0_20260131_002911/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 501012.5 | 498991.0 |
| 5 | 501012.7 | 498991.2 |
| 10 | 501013.2 | 498992.2 |
| 20 | 501013.2 | 498992.2 |
| 50 | 501012.8 | 498991.3 |

### Observations (E0)

- M1 noise-floor p95 stays below ~25 us across rates.
- M4 TimeSync/Piggyback remain ~0.50 s and flat across poll rates.

## E2_asymmetry_40ms

### M1 (Cristian baseline)

Source: `benchmarks/run_poll_rate_m1_e2_20260131_002932/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 19996.7 | 19999.3 |
| 5 | 20011.7 | 19998.0 |
| 10 | 20011.7 | 20000.0 |
| 20 | 20011.7 | 20000.0 |
| 50 | 20008.0 | 19999.0 |

### M4 (TimeSync)

Source: `benchmarks/run_poll_rate_m4_timesync_e2_20260131_002952/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 530053.3 | 489992.0 |
| 5 | 530080.0 | 489992.0 |
| 10 | 530157.3 | 489992.0 |
| 20 | 530157.3 | 489992.0 |
| 50 | 530152.0 | 489984.0 |

### M4 (Piggyback)

Source: `benchmarks/run_poll_rate_m4_piggyback_e2_20260131_003011/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 530308.0 | 489573.7 |
| 5 | 530308.3 | 489627.2 |
| 10 | 530308.3 | 489646.0 |
| 20 | 530308.3 | 489646.2 |
| 50 | 530308.0 | 489638.2 |

### Observations (E2)

- M1 stays near 20 ms p95 on both directions across poll rates.
- M4 TimeSync/Piggyback are flat and remain in the 0.49-0.53 s range.

## E3_bufferbloat

### M1 (Cristian baseline)

Source: `benchmarks/run_poll_rate_m1_e3_20260131_003029/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 542.0 | 451.3 |
| 5 | 853.0 | 677.7 |
| 10 | 898.3 | 734.0 |
| 20 | 898.3 | 734.0 |
| 50 | 898.3 | 734.0 |

### M4 (TimeSync)

Source: `benchmarks/run_poll_rate_m4_timesync_e3_20260131_003049/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 510506.7 | 489493.3 |
| 5 | 510509.3 | 489493.3 |
| 10 | 510605.3 | 489493.3 |
| 20 | 510605.3 | 489493.3 |
| 50 | 510541.3 | 489493.3 |

### M4 (Piggyback)

Source: `benchmarks/run_poll_rate_m4_piggyback_e3_20260131_003109/poll_rate_summary.csv`

| poll_rate_hz | mean_p95_ab_us | mean_p95_ba_us |
| --- | --- | --- |
| 2 | 510635.8 | 488966.5 |
| 5 | 510638.8 | 488971.0 |
| 10 | 510639.0 | 488985.0 |
| 20 | 510639.0 | 488985.0 |
| 50 | 510638.3 | 488975.3 |

### Observations (E3)

- M1 p95 increases from ~0.5 ms to ~0.9 ms as poll rate rises, then plateaus.
- M4 TimeSync/Piggyback stay flat around 0.49-0.51 s across poll rates.
