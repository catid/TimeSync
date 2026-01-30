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

## Report PDF

Generate a summary PDF with plots:

```bash
bench/.venv/bin/python bench/report_summary.py benchmarks/run_YYYYMMDD_HHMMSS benchmarks/report_summary.pdf
```

## Break analysis

Flag experiments that breach error thresholds or fail to synchronize:

```bash
bench/.venv/bin/python bench/analyze_breaks.py benchmarks/run_YYYYMMDD_HHMMSS/summary.csv benchmarks/breaks_report.txt
```

## Monte Carlo heatmaps

Run Monte Carlo sampling and generate metric-pair heatmaps:

```bash
./bench/run_montecarlo.sh
bench/.venv/bin/python bench/report_montecarlo.py benchmarks/run_mc_YYYYMMDD_HHMMSS/montecarlo.csv benchmarks/montecarlo_report.pdf
```
