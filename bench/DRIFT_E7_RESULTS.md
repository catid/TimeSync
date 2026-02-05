# E7 Drift Results (best variant per method)

| Method | Estimator | Discipline | p95 AB (us) | p95 BA (us) | Overhead (bps) | Run |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync (baseline) | min | none | 3458 | 3300 | 366 | benchmarks/run_peer_20260202_053217 |
| M4_TimeSyncSkew (best of grid) | median | fll | 4879 | 4720 | 366 | benchmarks/run_peer_20260202_053243 |
| M4_TimeSyncSkewReg (best of grid) | regress | none | 3574 | 3486 | 366 | benchmarks/run_peer_20260202_053243 |
| M4_TimeSyncAdaptive | min | none | 349 | 381 | 366 | benchmarks/run_peer_20260202_053353 |
| M4_TimeSyncDualWindow | regress | hybrid | 5504 | 5178 | 366 | benchmarks/run_peer_20260202_061032_e7_methods |
| M4_TimeSyncHysteresis | min | none | 2283 | 2253 | 366 | benchmarks/run_peer_20260202_061032_e7_methods |
| M4_TimeSyncSkewCorrected | regress | hybrid | 5513 | 5177 | 366 | benchmarks/run_peer_20260202_061032_e7_methods |
| M4_TimeSyncCUSUM | min | none | 420 | 2275 | 366 | benchmarks/run_peer_20260202_061032_e7_methods |
| M4_TimeSyncVarGate | min | none | 362 | 392 | 366 | benchmarks/run_peer_20260202_061032_e7_methods |
| M4_TimeSyncPerDirSkew | regress | hybrid | 3458 | 3300 | 374 | benchmarks/run_peer_20260202_061032_e7_methods |
| M4_TimeSyncStepReset | min | none | 462 | 2261 | 366 | benchmarks/run_peer_20260202_053417 |
| M4_TimeSyncProbe | regress | hybrid | 3280 | 3445 | 414 | benchmarks/run_peer_20260202_053442 |
| M4_Piggyback (best of grid) | pquant | none | 509807 | 489658 | 204 | benchmarks/run_peer_20260202_053503 |
| M1_Cristian (baseline) | median | hybrid | 3806 | 4134 | 392 | benchmarks/run_peer_20260202_053530 |
