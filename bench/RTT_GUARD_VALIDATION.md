# RTTGuard Validation

Goal: confirm RTT-sum guard signals are computed and exposed in CSV.

## Run

```
mkdir -p benchmarks/run_rtt_guard_validation_20260202_190200
./build/peer_bench \
  --scenario E6_path_change \
  --method TimeSync \
  --seeds 1 \
  --duration 40 \
  --threads 8 \
  --out benchmarks/run_rtt_guard_validation_20260202_190200/peer_bench.csv
```

## Result snapshot (M4_TimeSync)

From `benchmarks/run_rtt_guard_validation_20260202_190200/peer_bench.csv`:

- rtt_short_mean_us: 1.34262e+08  
- rtt_long_mean_us: 1.34255e+08  
- rtt_delta_mean_us: 7237.89  
- rtt_iqr_mean_us: 1971.37  
- rtt_guard_ok_ratio: 0

Notes:
- RTT delta and IQR rise during path change; guard ratio drops to 0 with default thresholds (200us), showing the guard catches instability.
