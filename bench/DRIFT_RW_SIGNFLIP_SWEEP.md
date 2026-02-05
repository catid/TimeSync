# Drift RW Sign-Flip Sweep (E85)

Run:
- `benchmarks/run_drift_rw_signflip_20260203/peer_bench.csv`
- `benchmarks/run_drift_rw_signflip_20260203/method_compare.md`

Scenarios:
- E7_drift, E7_drift_25ppm, E7_drift_100ppm, E7_drift_200ppm
- E85_drift_rw_signflip

E85 highlights (poll_time_err_p95, µs):
- `M4_TimeSync`: 4288
- `M4_TimeSyncShadow`: 4232
- `M4_TimeSyncShadowSkew`: 4232
- `M4_TimeSyncStepReset`: 617
- `M4_TimeSyncDDAC`: 5572

Notes:
- Sign-flip drift + random walk penalizes baseline and DDAC; StepReset is the only one with sub‑ms p95 here.
- Shadow variants track close to baseline; no major E7 improvements in this sweep.
