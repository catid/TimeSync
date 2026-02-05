# Fairness check: video sweep ext3

- Run: `benchmarks/run_video_sweep_20260203_ext3/peer_bench.csv`
- Command:
  `python3 bench/check_fairness.py --mindelta-us 1000000 --allow-probe-methods "M4_TimeSyncProbe,M4_TimeSyncStepGuard:probe,M4_TimeSyncStepGuard:probe_hard,M4_TimeSyncStepGuard:probe_hard_t100,M4_TimeSyncStepGuard:probe_hard_t5ms,M4_TimeSyncStepGuard:probe_innov,M4_TimeSyncStepGuard:probe_lowrate,M4_TimeSyncStepGuard:probe_mad10,M4_TimeSyncStepGuard:probe_mad4,M4_TimeSyncStepGuard:probe_mad6,M4_TimeSyncStepGuard:probe_only,M4_TimeSyncStepGuard:probe_relax" --max-probe-rate 1.0 benchmarks/run_video_sweep_20260203_ext3/peer_bench.csv`
- Output: `All M4_TimeSync rows use mindelta_interval_us=1000000`

Result: no fairness violations after allowing probe-specific variants at 1 Hz.
