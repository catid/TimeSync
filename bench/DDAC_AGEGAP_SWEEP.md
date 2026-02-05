# DDAC Age-Gap / Clamp Sweep (E53/E54)

Run: `bench/run_ddac_agegap_sweep.sh benchmarks/run_ddac_agegap_20260203`

Scenarios:
- E53_randstep_normal
- E54_drift_sign_flip

Methods:
- M4_TimeSync (baseline)
- M4_TimeSyncDDAC
- DDAC variants: age2s, age10s, clamp50, clampiqr0p5, clampiqr1p0, signonly

## Summary
- DDAC variants produced identical metrics on E53/E54 in this sweep.
- Baseline M4_TimeSync slightly outperformed DDAC on E54_drift_sign_flip (p95 error 3440 vs 4552 in method_compare).
- No evidence that age-gap or clamp tuning helped on E53/E54 under current gating.

## Next directions
- Inspect whether DDAC skew confidence gates are ever opening in E54; if not, age-gap/clamp tuning will have no effect.
- Consider enabling DDAC step handling (`:step`) for E53 random steps, or relax rtt guard for this scenario.
