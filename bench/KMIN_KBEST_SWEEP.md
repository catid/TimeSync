# KMin / KBest Sweep

Run: `benchmarks/run_kmin_kbest_20260203_091222` (SEEDS=1, scenarios E0/E4/E5/E7/E7_100ppm).

## Summary
- KMin/KBest variants are wired and produce outputs; use `method_compare.md` for per-scenario metrics.
- With a single seed, no clear winner is evident; longer sweep is required for stable ranking.

## Artifacts
- Method compare: `benchmarks/run_kmin_kbest_20260203_091222/method_compare.md`
- Merged CSV: `benchmarks/run_kmin_kbest_20260203_091222/peer_bench.csv`

---

# KMin / KBest Sweep (E7 + E63–E70)

Run: `benchmarks/run_kmin_kbest_e7_e63e70_20260203` (SEEDS=3, scenarios E7_drift + E63–E70).

## Summary
- KMin/KBest variants are **worse** than baseline on E7 (p95 ~4.6–5.7 ms vs 3.4 ms baseline).
- E63/E66 show low p95 (~8–56 µs) in some directions, but E64/E65 remain ~500 ms; large 120s steps remain huge.
- Variants are not baseline‑safe in this sweep (`within_baseline_E0_6 = no`).

## Artifacts
- Method compare: `benchmarks/run_kmin_kbest_e7_e63e70_20260203/method_compare.md`
- Merged CSV: `benchmarks/run_kmin_kbest_e7_e63e70_20260203/peer_bench.csv`
