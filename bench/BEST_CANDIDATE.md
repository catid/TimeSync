# Best-Candidate Summary (E0–E73)

Goal: baseline performance on E0–E6, best performance on E7, and no regressions in video/clock‑step cases (E10–E73).

## Current status (full run E0–E73, non‑grid)

- **Best E7 performers:** `M4_TimeSyncHysteresis:t100_c1` and `M4_TimeSyncAdaptive` (~**381 us** p95), with `M4_TimeSyncVarGate` close (~**392 us**), but they **regress E4/E5/E6** vs baseline.
- **Best E7 within baseline E0–E6:** `M4_TimeSyncShadow` / `M4_TimeSyncShadowSkew` (~**3454 us** p95), slightly better than baseline.
- **Video scenarios (E10–E13):** baseline‑compliant methods remain sub‑ms under congestion/bursts/path changes.
- **Clock steps (E14–E21 + E45):** baseline `M4_TimeSync` is ~67s p95 on large steps; Shadow variants reduce to ~15s, but large steps still dominate error budgets. E45 jump sweep shows similar large‑step dominance across baseline‑safe methods.
- **Random steps / drift flips (E53/E54):** E53 remains a major error contributor; 10‑seed sensitivity shows baseline/Shadow:step recover ~44–46s on average, while StepGuard/DDAC:step do not reach the 1ms recovery threshold (`bench/RANDSTEP_SENSITIVITY.md`). DDAC age‑gap/clamp sweep shows no measurable improvement in E53/E54 (`bench/DDAC_AGEGAP_SWEEP.md`).
- **New video cases (E55–E62):** E59 path‑flap, E60 correlated jitter, E61 congestion step, and E62 asym spike+step added; latest sweep `benchmarks/run_video_sweep_20260203_ext5` (see `bench/VIDEO_EXTENDED_SUMMARY.md`).
- **New clock steps (E63–E70):** 1s and 120s forward/back steps on A/B behave like E14–E21; huge steps remain unrecovered for baseline‑safe methods.
- **New video cases (E71–E73):** latency random‑walk, congestion ramp, and jitter‑burst scenarios added to the video sweep.

## Evidence

- E2–E6 comparisons:
  - `bench/E2_E6_ADAPTIVE_COMPARE.md`
  - `bench/E2_E6_VARGATE_COMPARE.md`
- E7 sweeps and follow‑ups:
  - `bench/DRIFT_E7_RESULTS.md`
  - `bench/DRIFT_IDEAS_E7.md`
- Full run:
  - `benchmarks/run_peer_full_20260203e/method_compare.md`
  - `benchmarks/run_peer_full_20260203e/e2e6_compare.md`
  - `benchmarks/run_peer_full_20260203e/e4e5_compare.md`
- Targeted sweeps:
  - `bench/MOE_STRICT_SWEEP.md`
  - `bench/AGECOMP_DDAC_CLAMP_SWEEP.md`
  - `bench/KMIN_SWEEP.md`
  - `bench/SHADOW_CSE_STEP_SWEEP.md`
  - `bench/STEPRESET_GUARDED_SWEEP.md`
  - `bench/DDAC_STEP_DIAGNOSTICS.md`
  - `bench/CLOCK_STEP_MC_SWEEP.md`
  - `bench/TOP_CONTENDERS_STEP_RECOVERY.md`
  - `bench/CLOCK_JUMP_REPORT.md`
  - `bench/TOP_CONTENDERS_VIDEO_NEW.md`
  - `bench/FULL_COMPARISON_20260203.md`
  - `bench/STEP_RECOVERY_SWEEP.md`
  - `bench/FAIRNESS_STEP_RECOVERY.md`
  - `bench/FAIRNESS_VIDEO_SWEEP_EXT3.md`
  - `bench/FOCUSED_CONTENDERS_20260203.md`
  - `bench/FOCUSED_CONTENDERS_20260203e.md`
  - `bench/FAIRNESS_FOCUSED_CONTENDERS.md`
  - `bench/STEP_GUARD_THRESHOLD_SWEEP.md`
  - `bench/DDAC_AGEGAP_SWEEP.md`
  - `bench/RANDSTEP_SENSITIVITY.md`
  - `bench/STEP_GUARD_PROBE_E53E45.md`
  - `bench/RANDSTEP_PROBE_SENSITIVITY.md`
  - `bench/MOE_E7_E53_SWEEP.md`

## Conclusion

No single method currently satisfies **baseline E0–E6**, **best‑in‑class E7**, and **robust clock‑step handling**. Adaptive/VarGate regress on E4/E5/E6; baseline TimeSync underperforms on E7; large steps remain a dominant failure mode.

Recent sweeps reaffirmed this:
- MoE strict gating removes E0 regression but does not improve E7.
- AgeComp/DD‑AC dynamic clamps did not activate often enough to change E7 outcomes; age‑gap/clamp variants still match baseline on E53/E54.
- KMin/KBest remain worse on E7 despite modest E5 gains.
- StepGuard threshold/gn_iqr tweaks did not improve random‑step or clock‑jump recovery in E53/E45.
- StepGuard probe variants still fail to recover on E53/E45 even at 10 ms thresholds; MoE gating variants do not improve E7/E53.

## Next steps (candidate directions)

1) **Conditional drift handling:** Detect E7‑like drift/step conditions and only then switch to drift‑tracking, otherwise remain baseline.
2) **Robust outlier handling:** Improve resistance to reorder/dup/loss (E4/E5) for adaptive methods.
3) **Fast step recovery:** Add RTT‑guarded step reset or promotion paths that respond in seconds (not L seconds).
4) **Random‑step resilience:** Add targeted handling for E53‑style random step times/sides without breaking E0–E6.
5) **Scenario‑specific tuning:** Validate E55–E58 (spike train + asym ramp + Pareto + spike/loss/reorder) and adjust guard thresholds for asymmetric jitter.
