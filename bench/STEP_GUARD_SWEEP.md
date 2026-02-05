# StepGuard Sweep (Step Detection + RTTGuarded Reset)

Goal: evaluate M4_TimeSyncStepGuard vs baseline on E0/E4/E5/E7.

## Run

```
SEEDS=2 GRID=1 THREADS=$(nproc) bench/run_stepguard_sweep.sh
```

Outputs:
- `benchmarks/run_stepguard_20260203_072706/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_072706/method_compare.md`

## Best StepGuard variants (p95 offset, avg of AB/BA)

From `benchmarks/run_stepguard_20260203_072706/method_compare.md`:

- E0_noise_floor: `M4_TimeSyncStepGuard:th0p5ms`  
  p95 AB/BA = 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: `M4_TimeSyncStepGuard:th0p5ms`  
  p95 AB/BA = 32.0 / 16.0 (avg 24.0)
- E5_reorder_dup: `M4_TimeSyncStepGuard:th0p5ms`  
  p95 AB/BA = 32.0 / 24.0 (avg 28.0)
- E7_drift: `M4_TimeSyncStepGuard` (base)  
  p95 AB/BA ≈ 4592 / 9236 (med max per method_compare)

Dynamic threshold variants (`iqr5`, `iqr10`) match base StepGuard in this sweep; no step recovery improvement.

## Baseline (M4_TimeSync) p95 offset for same seeds

From `benchmarks/run_stepguard_20260203_072706/method_compare.md`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3480 (median max per method_compare)

## Takeaways

- StepGuard improves E5 but **regresses E7** vs baseline in this sweep.
- Dynamic IQR thresholds (`iqr5`, `iqr10`) do not change step recovery metrics.

## Quick sanity sweep (E27/E28)

Run:
- `benchmarks/run_stepguard_20260203_083936/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_084258/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_084721/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_084915/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_085932/peer_bench.csv` (E0/E4/E5/E6/E7 + steps)

Notes:
- Includes `M4_TimeSyncStepGuard:nonear_p10`, `M4_TimeSyncStepGuard:rtt_step`,
  `M4_TimeSyncStepGuard:near_ratio`, and `M4_TimeSyncStepGuard:single_dir`.
- Quick scenarios show no delta vs baseline; intended only as a fast regression check.

## Update: E63–E70 (1s/120s steps)

Outputs:
- `benchmarks/run_stepguard_20260203_e63e70/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_e63e70/method_compare.md`

Highlights from `method_compare.md`:
- All StepGuard variants are identical on E63–E70.
- 1s steps: StepGuard reduces A‑fwd/B‑back p95 to ~8µs, but A‑back/B‑fwd remain at ~500ms (baseline‑like).
- 120s steps remain huge for all StepGuard variants; `M4_TimeSyncStepReset` is the only method with small p95 on 1s steps.
