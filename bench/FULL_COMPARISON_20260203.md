# Full comparison sweep (all E* scenarios)

- Run: `benchmarks/run_full_compare_20260203`
- Command: `SEEDS=1 THREADS=128 SCENARIO_FILTER="E" bench/run_peer_bench.sh benchmarks/run_full_compare_20260203`
- Outputs: `peer_bench.csv`, `method_compare.md` (PDF generation was slow; CSV + method_compare used for summary)

## Weighted score summary (lower is better)

Top 5 by weighted score:
- `M4_TimeSyncVarGate:robust` (0.367)
- `M4_TimeSyncStepReset:guard_xor` (0.419)
- `M4_TimeSyncAdaptiveGuard:s5` (0.419)
- `M4_TimeSyncAdaptiveGuard` (0.419)
- `M4_TimeSyncVarGate:hold500ms` (0.446)

These top scores are not baseline‑safe on E0–E6; they trade stability for E7 improvements.

## Baseline‑safe highlights (within_baseline_E0_6 == yes)

Sorted by E7 drift in this sweep:
- `M4_TimeSyncHybrid:t50_s5` (E7_drift=3448; E53_randstep_normal=837135; E54_drift_sign_flip=3451)
- `M4_TimeSyncSloped:w2s` (E7_drift=3448; E53_randstep_normal=837135; E54_drift_sign_flip=3451)
- `M4_TimeSyncHybrid:t25_s5` (E7_drift=3448; E53_randstep_normal=837135; E54_drift_sign_flip=3451)
- `M4_TimeSync` (E7_drift=3448; E53_randstep_normal=837135; E54_drift_sign_flip=3451)

## Observations
- Baseline‑safe contenders remain clustered near baseline on E7 drift in this run.
- Random step (E53) remains a major error contributor for baseline‑safe methods.
- Weighted‑score winners are primarily adaptive/gated variants that regress E0–E6.
