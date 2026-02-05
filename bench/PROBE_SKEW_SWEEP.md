# Probe Skew Calibration Sweep (E7 + E63–E70)

Run: `benchmarks/run_probe_skew_20260203_123312` (SEEDS=3, grid on)

Methods:
- M4_TimeSync
- M4_TimeSyncSkewCorrected
- M4_TimeSyncSkewCorrected:probe_skew_0p2hz

Key results (offset p95, µs):
- **E7_drift**: TimeSync 3448, SkewCorrected 5484, probe_skew_0p2hz 3488.
- **Steps E63–E66 (1s)**: probe_skew_0p2hz ~499.8k (same as baseline).
- **Steps E67–E70 (120s)**: unchanged vs baseline (no recovery; long-window stickiness).

Step recovery (median, seconds):
- **E63–E66**: TimeSync 9.1/7.1/9.1/10.1; probe_skew_0p2hz matches baseline.

Overhead:
- probe_skew_0p2hz 376 bps vs baseline 366 bps (low-rate probes add ~10 bps).

Conclusion:
- Low-rate probe skew improves over TimeSyncSkewCorrected, but does **not** beat baseline on E7 drift and does not improve step recovery.
