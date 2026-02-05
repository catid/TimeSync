# Drift-idea experiments (E7)

This note summarizes the E7 drift stress experiments for the proposed TimeSync
variants. See `bench/DRIFT_E7_RESULTS.md` for the full table of best-variant
metrics and run paths.

## Key results (E7)

- **Best overall:** M4_TimeSyncAdaptive (min/none) — p95 poll error **349/381 us** (AB/BA) at **366 bps**.
- **Baseline TimeSync:** 3.44/3.36 ms at 366 bps.
- **TimeSyncSkewReg (regress/none):** 3.57/3.49 ms at 366 bps.
- **TimeSyncSkew (median/FLL):** 4.88/4.72 ms at 366 bps.
- **TimeSyncProbe (regress/hybrid, 1 Hz probes):** 3.28/3.45 ms at 414 bps.
- **TimeSyncStepReset:** 0.46/2.26 ms at 366 bps (AB improves, BA still high).
- **Piggyback (quantile envelope):** ~0.49–0.51 s at 204 bps.
- **M1_Cristian baseline (1 Hz probes):** 3.81/4.13 ms at 392 bps.

## Notes on the ideas

1) **Skew tracking on TimeSync offsets (regression on MinDelta samples)**
   - Implemented as **M4_TimeSyncSkewReg**.
   - Improves vs TimeSyncSkew but does **not** beat baseline TimeSync.

2) **Min-quantile envelope**
   - Tested via **M4_Piggyback** quantile envelope variants.
   - Still large (~0.5 s) error in E7.

3) **Adaptive min-delta window**
   - Implemented as **M4_TimeSyncAdaptive**.
   - Large improvement in E7 with no overhead penalty.

4) **Low-rate probe for skew calibration**
   - Implemented as **M4_TimeSyncProbe** (1 Hz probes).
   - Did not improve E7 vs baseline; adds overhead.

5) **Step detection + window reset**
   - Implemented as **M4_TimeSyncStepReset**.
   - Helps one direction but still high BA error in E7.

6) **Hybrid discipline on TimeSync offsets**
   - Implemented as **M4_TimeSyncSkew** grid (PLL/FLL/Hybrid/Kalman).
   - No improvement vs baseline in E7.

## Additional variants (2026-02-02)

- **M4_TimeSyncDualWindow (regress/hybrid):** p95 **5504/5178 us** (AB/BA), overhead **366 bps**.
- **M4_TimeSyncSkewCorrected (regress/hybrid):** p95 **5513/5177 us**, overhead **366 bps**.
- **M4_TimeSyncHysteresis (min/none):** p95 **2283/2253 us**, overhead **366 bps**.
- **M4_TimeSyncCUSUM (min/none):** p95 **420/2275 us**, overhead **366 bps**.
- **M4_TimeSyncVarGate (min/none):** p95 **362/392 us**, overhead **366 bps**.
- **M4_TimeSyncPerDirSkew (regress/hybrid):** p95 **3458/3300 us**, overhead **374 bps**.

Run: `benchmarks/run_peer_20260202_061032_e7_methods/peer_bench.csv`

## Follow-up sweeps (2026-02-02)

- **VarGate hold variants:** `hold200ms` and `hold500ms` improve E1 jitter but regress E7 to **~2.28/2.25 ms**; keep default hold.  
  See `bench/VARGATE_HOLD_SWEEP.md`.
- **Adaptive trigger sweep:** `t50` matches default; `t200` introduces BA tail (**~2.275 ms**).  
  See `bench/ADAPTIVE_TRIGGER_SWEEP.md`.
- **Hysteresis sweep:** `t100_c1` matches best E7 (**~362/381 us**) but degrades E1; adding a 200ms hold helps E1 but reintroduces BA tail.  
  See `bench/HYST_CUSUM_SWEEP.md`.
- **CUSUM sweep:** hold variants greatly improve E1 (down to **~80/80 us**) but still leave E7 BA tail (**~2.25 ms**).  
  See `bench/HYST_CUSUM_SWEEP.md`.
- **Per-direction skew refinements:** sign-corrected peer skew + blend/gate/quantization do not improve E7.  
  See `bench/PERDIR_SKEW_SWEEP.md`.
- **Baseline FLL variant:** E7 remains poor (**~4.9/4.7 ms**).  
  See `bench/BASELINE_FLL_SWEEP.md`.

## Sanity checks for the best variant

For **M4_TimeSyncAdaptive** (best E7 variant):
- **E0 noise floor:** p95 0/0 us at 366 bps.
- **E1 stationary jitter:** p95 544/576 us at 366 bps.

These are comparable to the original TimeSync behavior in E0/E1.

## Scenario additions (2026-02-03)

- **E7_drift_ramp**: linear ppm ramp (0 → 200 ppm over ~50s) with no steps.
  Added for validating drift tracking without discrete steps.
