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
- `SUITE` (default suite or `testplan` for exhaustive coverage)
- `EXTRA_ARGS` (additional flags for `experiments`)
  - Example: `EXTRA_ARGS="--poll-rate-hz 5"` to override poll sampling rate
- `AUTO` (default: 1, enable 10-minute auto-scaling)
- `TARGET_MINUTES` (override auto target minutes)
- `CALIBRATE_SECONDS` (override calibration duration)
- `ASSERT` (set to 1 to enable UT-B..UT-E assertions)
- `BUILD_DIR` (default: `./build`)

Outputs are written under `benchmarks/run_YYYYMMDD_HHMMSS/` with one CSV per seed
and a `summary.csv` averaged across seeds.

To run the exhaustive test-plan suite:

```bash
SUITE=testplan SEEDS="1" JOBS=1 THREADS="$(nproc)" ./bench/run_experiments.sh
```

Auto-scaling to ~10 minutes is on by default; set `AUTO=0` to disable.

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

Environment overrides:
- `SAMPLES` (default: 1000)
- `THREADS` (default: nproc)
- `SEED` (default: 12345)
- `MC_FAMILY` / `MC_SEEDS` (family sweep, default seeds: 10)
- `AUTO` (default: 1, enable 10-minute auto-scaling)
- `BATCH=1` to disable auto/progress (event-driven batch mode)
- `POLL_RATE_HZ=5` to override application poll rate
- `TARGET_MINUTES` / `CALIBRATE_SECONDS` (auto-scaling tuning)
- `EXTRA_ARGS` (additional `experiments` flags)

Auto-scaling (default) calibrates for 10s and targets ~10 minutes; set `AUTO=0` to use fixed `SAMPLES`.

To run a named Monte Carlo family sweep (MC-F01..MC-F20):

```bash
MC_FAMILY=f01 MC_SEEDS=5 ./bench/run_montecarlo.sh
```

## Drift vs other metrics heatmaps

Coarse sweep to find the boundary region, then dense sweep for heatmaps:

```bash
./bench/run_drift_heatmaps.sh
```

## Scaling study

Measure Monte Carlo runtime vs thread count:

```bash
SAMPLES=10000 THREADS_LIST="1 2 4 8 16 32" ./bench/scale_threads.sh
```

## Peer-sync benchmark (M1–M4)

Run the peer-sync benchmark suite and generate the PDF report:

```bash
./bench/run_peer_bench.sh
```

Optional environment filters:
- `GRID=1` to sweep ablation grid (estimators/discipline/sampling)
- `TRAIN_ONLY=1` or `HOLDOUT_ONLY=1` for train/holdout split
- `SCENARIO_FILTER=E0` to limit scenarios
- `METHOD_FILTER=M1` to limit methods
- `DURATION=0.5` to override scenario duration (seconds)
- `POLL_RATE_HZ=10` to override application poll rate (Hz)

Outputs are written under `benchmarks/run_peer_YYYYMMDD_HHMMSS/` and include
`peer_bench.csv` and `peer_bench_report.pdf`.

## CPU utilization

See `bench/CPU_UTILIZATION.md` for sample CPU utilization snapshots on 128-thread runs.

## Poll-rate sweep

Sweep application poll rates and summarize poll-time error metrics:

```bash
./bench/poll_rate_sweep.sh
```

Optional environment overrides:
- `POLL_RATES="2 5 10 20 50"` (Hz values to sweep)
- `SCENARIO=E1` (scenario name)
- `METHOD=M2` (method name)
- `DURATION=1` (seconds)
- `SEEDS=3` (per-rate seeds)
- `THREADS=3` (per-process threads)

Outputs are written under `benchmarks/run_poll_rate_YYYYMMDD_HHMMSS/` and include
`poll_rate_*hz.csv` plus `poll_rate_summary.csv`.

See `bench/POLL_RATE_NOTES.md` for cross-method poll-rate sweep summaries.
