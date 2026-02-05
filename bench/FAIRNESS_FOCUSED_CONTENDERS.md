# Focused Contenders Fairness Check (2026-02-03)

Command:

```
python3 bench/check_fairness.py --mindelta-us 1000000 \
  --allow-probe-methods "M4_TimeSyncStepGuard:probe_hard,M4_TimeSyncStepGuard:probe_only,M4_TimeSyncStepGuard:probe_mad4,M4_TimeSyncStepGuard:probe,M4_TimeSyncStepGuard:probe_hard_t100,M4_TimeSyncStepGuard:probe_mad6,M4_TimeSyncStepGuard:probe_mad10,M4_TimeSyncStepGuard:probe_hard_t5ms,M4_TimeSyncProbe,M4_TimeSyncStepGuard:probe_lowrate,M4_TimeSyncStepGuard:probe_innov,M4_TimeSyncStepGuard:probe_relax" \
  --max-probe-rate 1.0 benchmarks/run_focused_contenders_20260203/peer_bench.csv
```

Result:

```
All M4_TimeSync rows use mindelta_interval_us=1000000
```

Notes:
- Probe variants are explicitly allowlisted; all of them use 1 Hz probes.
- Non-probe TimeSync methods are held to MinDelta=1s for apples-to-apples comparison.
