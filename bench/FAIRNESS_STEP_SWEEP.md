# Fairness check: step sweep

- Source CSV: `benchmarks/run_step_only_20260203_093715/peer_bench.csv`
- Command: `python3 bench/check_fairness.py --mindelta-us 1000000 --allow-probe-methods M4_TimeSyncProbe,... --max-probe-rate 1.0`

## Result
- All M4_TimeSync rows use `mindelta_interval_us=1000000`.
- Probe methods allowed at `probe_rate_hz <= 1.0`.
