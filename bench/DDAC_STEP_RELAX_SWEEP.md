# DD-AC Step Gate Relaxation Sweep

Goal: relax DD-AC step gating (drop IQR stability requirement) to improve step recovery without regressing E0–E6.

## Run

```
SEEDS=2 GRID=1 THREADS=$(nproc) bench/run_ddac_step_relax.sh
```

Outputs:
- `benchmarks/run_ddac_step_relax_20260203_072525/peer_bench.csv`
- `benchmarks/run_ddac_step_relax_20260203_072525/method_compare.md`

## Summary

From `method_compare.md`:
- `M4_TimeSyncDDAC:step_relax` matches `M4_TimeSyncDDAC:step` across E0–E6 and E14–E21.
- No measurable improvement in step recovery; metrics are identical to the existing step variants.

## Interpretation

Relaxing the stability/IQR gate alone is insufficient. Step recovery remains bounded by other gates (likely RTT guard / near‑floor), or the step condition itself is not firing under current thresholds.
