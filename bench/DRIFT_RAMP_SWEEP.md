# Drift Ramp Sweep (E7_drift_ramp)

Run:
- `benchmarks/run_drift_ramp_20260203_085029/peer_bench.csv`
- Methods: `M4_TimeSync`, `M4_TimeSyncShadow`, `M4_TimeSyncShadowSkew`, `M4_TimeSyncStepReset`, `M4_TimeSyncDDAC`

Key p95 poll error (µs) from `method_compare.md`:
- **M4_TimeSyncStepReset**: 944 (best in this sweep)
- **M4_TimeSyncShadow / ShadowSkew**: 3296
- **M4_TimeSync**: 3361
- **M4_TimeSyncDDAC**: 11880 (worst in this sweep)

Takeaway:
- StepReset dominates drift‑ramp in this limited sweep; Shadow variants slightly improve over baseline.
