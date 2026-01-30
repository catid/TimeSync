# Bench Harness

This directory contains helpers to run the TimeSync simulation experiments
with multiple seeds and aggregate results into a summary CSV.

## Run

```bash
./bench/run_experiments.sh
```

Environment options:
- `SEEDS` (default: "1 2 3 4 5")
- `JOBS` (default: nproc)
- `THREADS` (per-process threads, default: 1)
- `MATCH` or `ONLY` (filter experiments)
- `BUILD_DIR` (default: `./build`)

Outputs are written under `benchmarks/run_YYYYMMDD_HHMMSS/` with one CSV per seed
and a `summary.csv` averaged across seeds.
