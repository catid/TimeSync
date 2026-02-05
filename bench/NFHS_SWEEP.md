# NFHS Staleness Invalidation Sweep

Goal: evaluate M4_TimeSyncNFHS (near-floor hit staleness) vs baseline on E0/E4/E5/E7.

## Runs

```
for sc in E0_noise_floor E4_loss_burst E5_reorder_dup E7_drift; do
  dur=20; if [ "$sc" = "E7_drift" ]; then dur=30; fi
  ./build/peer_bench --scenario "$sc" --method "M4_TimeSyncNFHS" \
    --seeds 2 --duration "$dur" --threads 8 \
    --out benchmarks/run_nfhs_${sc}_20260202_201500/peer_bench.csv
done
```

Baseline references are the same runs as in `bench/SHADOW_WINDOW_SWEEP.md`:
`benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`.

## Best NFHS variants (p95 offset, avg of AB/BA)

From `benchmarks/run_nfhs_*_20260202_201500/peer_bench.csv`:

- E0_noise_floor: `M4_TimeSyncNFHS:miss8s`  
  p95 AB/BA = 0.0 / 0.0 (avg 0.0)
- E4_loss_burst: `M4_TimeSyncNFHS:miss8s`  
  p95 AB/BA = 32.0 / 16.0 (avg 24.0)
- E5_reorder_dup: `M4_TimeSyncNFHS:miss8s`  
  p95 AB/BA = 32.0 / 24.0 (avg 28.0)
- E7_drift: `M4_TimeSyncNFHS` (base)  
  p95 AB/BA = 4304.0 / 4516.0 (avg 4410.0)

## Baseline (M4_TimeSync) p95 offset for same seeds

From `benchmarks/run_gsp_*_baseline_20260202_200500/peer_bench.csv`:

- E0_noise_floor: 7.0 / 7.0 (avg 7.0)
- E4_loss_burst: 22.0 / 26.0 (avg 24.0)
- E5_reorder_dup: 101.0 / 116.0 (avg 108.5)
- E7_drift: 3478.0 / 3245.0 (avg 3361.5)

## Takeaways

- NFHS matches baseline on E4 and improves E5, but **regresses E7** relative to baseline.
- The miss8s variant is most stable on E0/E4/E5 in this sweep.
