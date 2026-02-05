# AgeComp/DDAC Dynamic Clamp Sweep

Goal: add dynamic clamp margins (scaled by short-window IQR) for AgeComp and DD-AC and verify impact on E7 drift while keeping E4/E5 stable.

## Run

```
SEEDS=2 GRID=1 THREADS=$(nproc) bench/run_agecomp_ddac_clamp.sh
```

Outputs:
- `benchmarks/run_agecomp_ddac_clamp_20260203_071622/peer_bench.csv`
- `benchmarks/run_agecomp_ddac_clamp_20260203_071622/method_compare.md`

## Variants

- AgeComp: `clampiqr0p5` (max(50us, 0.5*IQR)), `clampiqr1p0` (max(100us, 1.0*IQR))
- DD-AC: `clampiqr0p5`, `clampiqr1p0` with same rule

## Summary

From `method_compare.md` (E0/E4/E5/E7 subset):
- AgeComp dynamic clamp variants matched baseline AgeComp metrics across E0/E4/E5/E7 in this run.
- DD-AC dynamic clamp variants matched baseline DD-AC metrics across E0/E4/E5/E7 in this run.
- No measurable E7 drift improvement observed; E4/E5 remained within baseline tolerance.

## Notes

- The clamp appears rarely active in the current scenarios (AgeComp/DD-AC behavior unchanged).
- If we want the clamp to engage more often, consider higher drift rates, longer windows, or lower clamp margins.
