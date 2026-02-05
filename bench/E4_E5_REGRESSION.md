# E4-E5 Regression Comparison

Source: benchmarks/run_peer_fullgrid_20260202_062329/peer_bench.csv
Methods: M4_TimeSync, M4_TimeSyncAdaptive, M4_TimeSyncCUSUM, M4_TimeSyncDualWindow, M4_TimeSyncHysteresis, M4_TimeSyncPerDirSkew, M4_TimeSyncProbe, M4_TimeSyncSkew, M4_TimeSyncSkewCorrected, M4_TimeSyncSkewReg, M4_TimeSyncStepReset, M4_TimeSyncVarGate

| scenario | method | p95_ab_us | p95_ba_us | p95_max_us | overhead_bps | rows |
| --- | --- | --- | --- | --- | --- | --- |
| E4_loss_burst | M4_TimeSync | 16 | 16 | 16 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncAdaptive | 552 | 488 | 552 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncCUSUM | 128 | 136 | 136 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncDualWindow | 34297 | 30976 | 34297 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncHysteresis | 40 | 32 | 40 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncPerDirSkew | 16 | 16 | 16 | 374 | 5 |
| E4_loss_burst | M4_TimeSyncProbe | 242 | 373 | 373 | 414 | 5 |
| E4_loss_burst | M4_TimeSyncSkew | 526 | 450 | 526 | 366 | 20 |
| E4_loss_burst | M4_TimeSyncSkewCorrected | 34320 | 30965 | 34320 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncSkewReg | 91257 | 90917 | 91257 | 366 | 15 |
| E4_loss_burst | M4_TimeSyncStepReset | 96 | 56 | 96 | 366 | 5 |
| E4_loss_burst | M4_TimeSyncVarGate | 456 | 488 | 488 | 366 | 5 |
| E5_reorder_dup | M4_TimeSync | 160 | 152 | 160 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncAdaptive | 5000 | 5032 | 5032 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncCUSUM | 5000 | 5032 | 5032 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncDualWindow | 2488 | 2194 | 2488 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncHysteresis | 4888 | 4904 | 4904 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncPerDirSkew | 160 | 152 | 160 | 374 | 5 |
| E5_reorder_dup | M4_TimeSyncProbe | 1016 | 877 | 1016 | 414 | 5 |
| E5_reorder_dup | M4_TimeSyncSkew | 1978 | 886 | 1978 | 366 | 20 |
| E5_reorder_dup | M4_TimeSyncSkewCorrected | 2521 | 2185 | 2521 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncSkewReg | 89850 | 86610 | 89850 | 366 | 15 |
| E5_reorder_dup | M4_TimeSyncStepReset | 5000 | 5032 | 5032 | 366 | 5 |
| E5_reorder_dup | M4_TimeSyncVarGate | 4984 | 5032 | 5032 | 366 | 5 |