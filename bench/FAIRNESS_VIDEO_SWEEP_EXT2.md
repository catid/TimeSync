# Fairness check: extended video sweep (E51/E52)

- Source CSV: `benchmarks/run_video_sweep_ext2_20260203_101005/peer_bench.csv`
- Command: `python3 bench/check_fairness.py --mindelta-us 1000000 --allow-probe-methods ... --max-probe-rate 1.0`

## Result
- All `M4_TimeSync*` rows use `mindelta_interval_us=1000000`.
- Probe methods allowed at `probe_rate_hz <= 1.0`.
