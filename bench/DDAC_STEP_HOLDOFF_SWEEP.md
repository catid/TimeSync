# Step Scenario Compare (E14–E21)

Source: benchmarks/run_ddac_step_holdoff_20260203_064813/peer_bench.csv

| method | estimator | discipline | recovery_med_s | E14_clock_step_small | E15_clock_step_large | E16_clock_step_small_a_fwd | E17_clock_step_small_a_back | E18_clock_step_large_a_fwd | E19_clock_step_large_a_back | E20_clock_step_small_b_back | E21_clock_step_large_b_back |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 7.72 | 1000005 | 67108900 | 1000010 | 1000020 | 18446700000000000000 | 18446700000000000000 | 1000005 | 18446700000000000000 |
| M4_TimeSyncDDAC:step_hold10s | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC:step_hold0 | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC:step_hold5s | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC:step_clamp200 | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC:step_clamp50 | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC:step_hold1s | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncDDAC:step | min | none | 62.00 | 1000035 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |

## Step Recovery Times (seconds, median across seeds)

| method | estimator | discipline | E14_clock_step_small | E15_clock_step_large | E16_clock_step_small_a_fwd | E17_clock_step_small_a_back | E18_clock_step_large_a_fwd | E19_clock_step_large_a_back | E20_clock_step_small_b_back | E21_clock_step_large_b_back |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 7.10 | 1.10 | 7.10 | 9.60 | 1.10 | 8.35 | 9.60 | 8.75 |
| M4_TimeSyncDDAC:step_hold10s | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC:step_hold0 | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC:step_hold5s | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC:step_clamp200 | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC:step_clamp50 | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC:step_hold1s | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |
| M4_TimeSyncDDAC:step | min | none | -1.00 | 32.00 | -1.00 | -1.00 | 62.00 | 62.00 | -1.00 | 90.00 |