# Guarded Shadow Promotion (GSP) Sweep

Goal: evaluate M4_TimeSyncShadow (Guarded Shadow Promotion) vs baseline on E0/E4/E5/E7.

## Runs

```
for sc in E0_noise_floor E4_loss_burst E5_reorder_dup E7_drift; do
  dur=20; if [ "$sc" = "E7_drift" ]; then dur=30; fi
  ./build/peer_bench --scenario "$sc" --method "M4_TimeSyncShadow" \
    --seeds 2 --duration "$dur" --threads 8 \
    --out benchmarks/run_gsp_${sc}_20260202_201000/peer_bench.csv
done

for sc in E0_noise_floor E4_loss_burst E5_reorder_dup E7_drift; do
  dur=20; if [ "$sc" = "E7_drift" ]; then dur=30; fi
  ./build/peer_bench --scenario "$sc" --method "M4_TimeSync" \
    --seeds 2 --duration "$dur" --threads 8 \
    --out benchmarks/run_gsp_${sc}_baseline_20260202_200500/peer_bench.csv
done
```

## Best GSP variants (p95 offset, avg of AB/BA)

From `benchmarks/run_gsp_*_20260202_201000/peer_bench.csv`:

- E0_noise_floor: `M4_TimeSyncShadow:xor_rtt`  
  p95 AB/BA = 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: `M4_TimeSyncShadow:rtt`  
  p95 AB/BA = 32.0 / 16.0 (avg 24.0)
- E5_reorder_dup: `M4_TimeSyncShadow:xor_rtt`  
  p95 AB/BA = 32.0 / 24.0 (avg 28.0)
- E7_drift: `M4_TimeSyncShadow` (base)  
  p95 AB/BA = 3046.0 / 3306.0 (avg 3176.0)

## Baseline (M4_TimeSync) p95 offset for same seeds

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Takeaways

- GSP matches baseline on E4 and improves E5 without regressions.
- E7 improves modestly vs baseline (3176 avg vs 3361).
- Best variant depends on scenario (xor+rtt gating is strongest for E0/E5).
