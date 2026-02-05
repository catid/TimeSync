# Peer Bench Full Grid Summary

Source: benchmarks/run_peer_full_20260203e/peer_bench.csv

## Latest full grid (E0–E73, grid)

Key findings from `benchmarks/run_peer_full_20260203e/method_compare.md`:

- **Best E7 overall:** `M4_TimeSyncAdaptive`, `M4_TimeSyncVarGate`, and `M4_TimeSyncHysteresis:t100_c1` (~428 µs p95) but **still regress E4/E5/E6** vs baseline.
- **Best E7 within baseline E0–E6:** `M4_TimeSyncShadow` / `M4_TimeSyncShadowSkew` (~3470 µs), slightly better than baseline.
- **Video scenarios (E10–E13 + E71–E73):** baseline‑safe methods remain low‑error; baseline `M4_TimeSync` p95 is ~9.7ms/2.9ms/0.12ms on E71/E72/E73.
- **Clock steps (E14–E21 + E63–E70):** baseline `M4_TimeSync` reaches ~0.5s p95 on 1s steps and ~134s on 120s steps; `M4_TimeSyncStepReset` reduces 1s steps to ~0.1–0.2ms but huge 120s steps remain dominant.

See:
- `benchmarks/run_peer_full_20260203e/method_compare.md`
- `benchmarks/run_peer_full_20260203e/e2e6_compare.md`
- `benchmarks/run_peer_full_20260203e/e4e5_compare.md`
- `benchmarks/run_peer_full_20260203e/peer_bench_report.pdf`
- `bench/MOE_STRICT_SWEEP.md`
- `bench/AGECOMP_DDAC_CLAMP_SWEEP.md`
- `bench/KMIN_SWEEP.md`
- `bench/SHADOW_CSE_STEP_SWEEP.md`
- `bench/STEPRESET_GUARDED_SWEEP.md`
- `bench/DDAC_STEP_DIAGNOSTICS.md`
- `bench/CLOCK_STEP_MC_SWEEP.md`

## Top-contender sweep (E0–E21 subset)

Source: `benchmarks/run_top_contenders_20260203_061442/peer_bench.csv`

Median p95 poll-time error (µs). Values ≥1e12 indicate overflow / effective failure.

| method | E7 | E14 | E15 | E16 | E17 | E18 | E19 | E20 | E21 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | 3458 | 1000010 | 67108900 | 1000010 | 1000020 | ≥1e12 | ≥1e12 | 1000010 | ≥1e12 |
| M4_TimeSyncShadow | 3454 | 1000010 | 15000000 | 1000000 | 1000020 | 15000000 | 15000000 | 1000000 | 15000000 |
| M4_TimeSyncVarGate | 392 | 240 | 67109200 | 248 | 264 | ≥1e12 | ≥1e12 | 264 | ≥1e12 |
| M4_TimeSyncAdaptive | 381 | 264 | 67109200 | 256 | 264 | ≥1e12 | ≥1e12 | 264 | ≥1e12 |

Notes:
- VarGate/Adaptive win on E7 but blow up on large A/B steps (E18/E19/E21) and regress E4/E5/E6 in the full grid.
- Shadow variants preserve E0–E6 baseline and reduce large-step error to ~15s, but large steps still dominate error budgets.

## Candidate status (2026-02-03)

Goal: baseline performance on E0-E6 and best performance on E7.

Findings from targeted E2-E6 comparisons (5 seeds):
- **M4_TimeSyncAdaptive** and **M4_TimeSyncVarGate** match E7 best‑in‑class but **regress E4/E5/E6** vs baseline.
- **Baseline M4_TimeSync** remains the only method matching E0‑E6 across those checks.

See `bench/E2_E6_ADAPTIVE_COMPARE.md` and `bench/E2_E6_VARGATE_COMPARE.md`.

TS24 plausibility filter sweep (`bench/PLAUSIBILITY_FILTER_SWEEP.md`):
- With NEG_EPS ≤ 200us (current sweep), the negative‑delta direction is fully filtered out under the 1s offset regime.
- No viable filter settings yet; needs a larger NEG_EPS or different unwrap bias.

Guarded Shadow Promotion sweep (`bench/SHADOW_WINDOW_SWEEP.md`):
- GSP matches baseline on E4, improves E5, and modestly improves E7 without regressions in this small sweep.

NFHS sweep (`bench/NFHS_SWEEP.md`):
- NFHS improves E5 but regresses E7 vs baseline.

StepGuard sweep (`bench/STEP_GUARD_SWEEP.md`):
- StepGuard improves E5 but regresses E7 vs baseline.

Consensus-slope sweep (`bench/CONSENSUS_SLOPE_SWEEP.md`):
- Consensus-slope improves E4/E5 vs baseline but regresses E7 by default; disabling RTT guard improves E7 but causes severe E4/E5 regressions.

Shadow+Skew sweep (`bench/SHADOW_SKEW_SWEEP.md`):
- Shadow+Skew improves E4/E5 and slightly improves E7 vs baseline; variants collapsed to identical metrics.

Tilted-min sweep (`bench/TILTED_MIN_SWEEP.md`):
- Tilted-min improves E4/E5 but regresses E7 by default; disabling RTT guard (noguard) activates CSE and improves E7 with mild E1/E4/E6 regressions.
- Additional CSE gating sweeps (noguard_strict/net2) reduce E1 but still regress E6 and do not preserve E7 gains under guarded settings.

Age-compensated minima sweep (`bench/ACM_SWEEP.md`):
- ACM improves E4/E5 but regresses E7; variants collapsed to identical metrics (CSE likely inactive).

KMin/KBest sweep (`bench/KMIN_SWEEP.md`):
- KMin regresses E5/E7; KBest matches the collapsed pattern (E4/E5 good, E7 regression).

MoE sweep (`bench/MOE_SWEEP.md`):
- Base MoE regresses E0; XOR-gated variant restores E0 but still regresses E7.
- MoE tuning (resid clamp/guard changes) did not improve E7; base variant still exhibits BA spikes.

MoE strict sweep (`bench/MOE_STRICT_SWEEP.md`):
- Strict gating + XOR removes E0 regression but does **not** improve E7 vs baseline.

DD-AC sweep (`bench/DD_AC_SWEEP.md`):
- DD-AC improves E4/E5 but regresses E7; noguard slightly better in E7 but still worse than baseline.

AgeComp/DDAC clamp sweep (`bench/AGECOMP_DDAC_CLAMP_SWEEP.md`):
- Dynamic clamp (IQR-scaled) does not change E7 outcomes in current scenarios (clamp rarely active).

## Top 10 by poll_time_err_p95_ab_us (median)

| rank | method | estimator | discipline | poll_p95_ab_med_us | poll_p95_ab_max_us | poll_p95_ba_med_us | overhead_med_bps | e8_over_budget_pct | teleop_rms_med | rows |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | M4_TimeSyncPerDirSkew | regress | hybrid | 24.0 | 515032.0 | 16.0 | 374.0 | 0.0% | 0.294 | 50 |
| 2 | M4_TimeSync | min | none | 24.0 | 515032.0 | 16.0 | 366.0 | 0.0% | 0.294 | 50 |
| 3 | M4_TimeSyncSkew | median | fll | 39.0 | 515169.0 | 14.5 | 366.0 | 0.0% | 0.294 | 50 |
| 4 | M4_TimeSyncHysteresis | min | none | 64.0 | 515088.0 | 40.0 | 366.0 | 0.0% | 0.294 | 50 |
| 5 | M4_TimeSyncStepReset | min | none | 144.0 | 515088.0 | 96.0 | 366.0 | 0.0% | 0.294 | 50 |
| 6 | M4_TimeSyncCUSUM | min | none | 180.0 | 515216.0 | 140.0 | 366.0 | 0.0% | 0.294 | 50 |
| 7 | M2_NTP | median | hybrid | 371.0 | 20322.0 | 530.5 | 408.0 | 100.0% | 0.299 | 50 |
| 8 | M1_Cristian | median | hybrid | 371.0 | 20322.0 | 530.5 | 392.0 | 100.0% | 0.299 | 50 |
| 9 | M3_PTP | median | hybrid | 371.0 | 20322.0 | 530.5 | 408.0 | 100.0% | 0.299 | 50 |
| 10 | M3_PTP | regress | hybrid | 396.5 | 20832.0 | 378.5 | 408.0 | 100.0% | 0.299 | 50 |

## Full Method Grid (median across all rows)

| method | estimator | discipline | poll_p95_ab_med_us | poll_p95_ba_med_us | owd_p95_ab_med_us | owd_p95_ba_med_us | overhead_med_bps | over_budget_pct | e8_over_budget_pct | teleop_rms_med | rows |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| M1_Cristian | median | hybrid | 371.0 | 530.5 | 2000075.0 | 2000020.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | median | kalman | 783.5 | 739.5 | 2000115.0 | 2000085.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | median | none | 686.5 | 752.5 | 2000210.0 | 2000125.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | min | hybrid | 529.5 | 553.0 | 2000145.0 | 1999985.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | min | kalman | 820.5 | 1104.0 | 2000120.0 | 2000005.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | min | none | 758.0 | 790.5 | 2000210.0 | 2000005.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | regress | hybrid | 396.5 | 378.5 | 2000040.0 | 2000060.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | regress | kalman | 750.0 | 686.0 | 2000155.0 | 2000160.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M1_Cristian | regress | none | 813.0 | 767.5 | 2000175.0 | 2000195.0 | 392.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | median | hybrid | 371.0 | 530.5 | 2000075.0 | 2000000.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | median | kalman | 783.5 | 739.5 | 2000120.0 | 2000085.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | median | none | 686.5 | 752.5 | 2000210.0 | 2000125.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | min | hybrid | 529.5 | 553.0 | 2000145.0 | 1999980.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | min | kalman | 820.5 | 1104.0 | 2000120.0 | 2000000.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | min | none | 758.0 | 790.5 | 2000210.0 | 2000000.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | regress | hybrid | 396.5 | 378.5 | 2000050.0 | 2000060.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | regress | kalman | 750.0 | 686.0 | 2000155.0 | 2000160.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M2_NTP | regress | none | 813.0 | 767.5 | 2000180.0 | 2000195.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | median | hybrid | 371.0 | 530.5 | 2000075.0 | 2000000.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | median | kalman | 783.5 | 739.5 | 2000120.0 | 2000085.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | median | none | 686.5 | 752.5 | 2000210.0 | 2000125.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | min | hybrid | 529.5 | 553.0 | 2000145.0 | 1999980.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | min | kalman | 820.5 | 1104.0 | 2000120.0 | 2000000.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | min | none | 758.0 | 790.5 | 2000210.0 | 2000000.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | regress | hybrid | 396.5 | 378.5 | 2000050.0 | 2000060.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | regress | kalman | 750.0 | 686.0 | 2000155.0 | 2000160.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M3_PTP | regress | none | 813.0 | 767.5 | 2000180.0 | 2000195.0 | 408.0 | 10.0% | 100.0% | 0.299 | 50 |
| M4_Piggyback | pquant | hybrid | 510226.0 | 489955.0 | 1510150.0 | 2000000.0 | 186.0 | 0.0% | 0.0% | 0.388 | 200 |
| M4_Piggyback | pquant | kalman | 510151.0 | 489962.5 | 1510120.0 | 2000290.0 | 186.0 | 0.0% | 0.0% | 0.388 | 200 |
| M4_Piggyback | pquant | none | 510151.0 | 489962.0 | 1510120.0 | 2000000.0 | 186.0 | 0.0% | 0.0% | 0.388 | 200 |
| M4_TimeSync | min | none | 24.0 | 16.0 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncAdaptive | min | none | 425.5 | 375.5 | 351.5 | 339.5 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncCUSUM | min | none | 180.0 | 140.0 | 124.0 | 132.5 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncDualWindow | regress | hybrid | 2531.5 | 2137.0 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncHysteresis | min | none | 64.0 | 40.0 | 48.0 | 48.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncPerDirSkew | regress | hybrid | 24.0 | 16.0 | 20.0 | 26.0 | 374.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncProbe | regress | hybrid | 458.0 | 505.5 | 21.0 | 25.0 | 414.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkew | median | fll | 39.0 | 14.5 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkew | median | hybrid | 3782.0 | 2889.5 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkew | median | kalman | 483.5 | 453.0 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkew | median | pll | 3782.0 | 2889.5 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkewCorrected | regress | hybrid | 2531.5 | 2137.0 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkewReg | regress | hybrid | 95072.5 | 91616.0 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkewReg | regress | kalman | 87022.5 | 81645.0 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncSkewReg | regress | none | 86927.0 | 80757.5 | 20.0 | 26.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncStepReset | min | none | 144.0 | 96.0 | 88.0 | 102.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncVarGate | min | none | 420.0 | 375.5 | 347.0 | 330.0 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncVarGate:t100_v20k_w20 | min | none | 425.5 | 376.0 | 351.0 | 336.5 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncVarGate:t150_v50k_w40 | min | none | 480.0 | 352.0 | 341.0 | 321.5 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |
| M4_TimeSyncVarGate:t50_v5k_w10 | min | none | 433.5 | 375.5 | 349.0 | 336.5 | 366.0 | 0.0% | 0.0% | 0.294 | 50 |

Notes:
- poll_p95 values are medians across all scenario/seed rows in the full grid.
- over_budget_pct is percent of rows with over_budget=1 across all scenarios; e8_over_budget_pct is E8-only.
- teleop_rms_med is median teleop RMS across E9 rows; 0 indicates no teleop samples for that method.
