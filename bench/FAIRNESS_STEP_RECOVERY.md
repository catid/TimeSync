# Step recovery fairness check

- Run: `python3 bench/check_fairness.py --mindelta-us 1000000 --allow-probe-methods M4_TimeSyncStepGuard --max-probe-rate 1.0 benchmarks/run_step_recovery_20260203_101549/peer_bench.csv`
- Output: `All M4_TimeSync rows use mindelta_interval_us=1000000`
- Result: no fairness violations flagged for the step recovery sweep.
