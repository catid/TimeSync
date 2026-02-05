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

## Ranking table

Generate a composite ranking table from a peer_bench sweep:

```bash
./bench/run_ranking_table.sh benchmarks/run_YYYYMMDD_HHMMSS/peer_bench.csv bench/RANKING_TABLE.md
```

Generate paired leaderboards for with-drift vs no-drift runs:

```bash
./bench/report_core_leaderboards.py --with-drift benchmarks/run_peer_with_drift/peer_bench.csv \
  --no-drift benchmarks/run_peer_no_drift/peer_bench.csv --out bench/CORE_LEADERBOARDS.md
```

Stratify results by skew buckets:

```bash
./bench/report_skew_buckets.py benchmarks/run_YYYYMMDD_HHMMSS/peer_bench.csv --out bench/SKEW_BUCKETS.md
```

Bootstrap confidence intervals for scenario medians:

```bash
./bench/report_bootstrap_ci.py benchmarks/run_YYYYMMDD_HHMMSS/peer_bench.csv --out bench/BOOTSTRAP_CI.md
```

## Break analysis

Flag experiments that breach error thresholds or fail to synchronize:

```bash
bench/.venv/bin/python bench/analyze_breaks.py benchmarks/run_YYYYMMDD_HHMMSS/summary.csv benchmarks/breaks_report.txt
```

## Budget analysis

List over-budget rows in peer_bench outputs:

```bash
./bench/analyze_budget.py benchmarks/run_peer_YYYYMMDD_HHMMSS/peer_bench.csv --summary
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

To run a named Monte Carlo family sweep (MC-F01..MC-F22, including `video` and `clock-step`):

```bash
MC_FAMILY=f01 MC_SEEDS=5 ./bench/run_montecarlo.sh
MC_FAMILY=video MC_SEEDS=5 ./bench/run_montecarlo.sh
MC_FAMILY=clock-step MC_SEEDS=5 ./bench/run_montecarlo.sh
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

## Peer-sync benchmark (M1–M4 + TimeSyncSkew)

Run the peer-sync benchmark suite and generate the PDF report:

```bash
./bench/run_peer_bench.sh
```

## Top-contender sweep (Shadow/VarGate/Adaptive vs baseline)

Run a focused sweep across core + video + step scenarios with scoring:

```bash
./bench/run_top_contenders.sh
```

## Top-contender parameter sweep (custom methods/scenarios)

Run a customizable sweep with composite score + Pareto frontier outputs:

```bash
./bench/run_top_contender_sweep.sh
```

## Step-scenario acceptance (E14–E21)

Run a focused acceptance + step recovery report for clock-step scenarios:

```bash
./bench/steps_acceptance.sh
```

Optional environment filters:
- `GRID=1` to sweep ablation grid (estimators/discipline/sampling)
- `TRAIN_ONLY=1` or `HOLDOUT_ONLY=1` for train/holdout split
- `PROFILE=core|skew|low-jitter-long` to select scenario profile (`skew` excludes stress; `low-jitter-long` picks >=60s and <=1ms jitter each direction)
- `SCENARIO_FILTER=E0` to limit scenarios
- `METHOD_FILTER=M1` to limit methods
- `DURATION=0.5` to override scenario duration (seconds)
- `POLL_RATE_HZ=10` to override application poll rate (Hz)
- `PROBE_RATE_HZ=5` to override probe rate for probe-based methods (Hz)

Defaults for fairness: probe-based methods use a 1 Hz probe rate (1 s sync interval),
and TimeSync/Piggyback exchange MinDelta every 1 s.

Outputs are written under `benchmarks/run_peer_YYYYMMDD_HHMMSS/` and include
`peer_bench.csv` and `peer_bench_report.pdf`.

Additional video-link scenarios (E10–E15) are documented in
`bench/VIDEO_SCENARIOS.md`.

`peer_bench.csv` includes poll-rate and budget context:
`poll_rate_hz`, `probe_rate_hz`, `overhead_budget_bps`, `budget_margin_bps`,
`over_budget`, plus poll-time error sample counts
(`poll_time_err_count_ab`/`poll_time_err_count_ba`) and validity flags.
Core error metrics are reported per direction (AB/BA columns) for offset, poll-time,
skew, and one-way delay, so both endpoints are tracked in the same run.

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
- `PROBE_RATE_HZ=5` (override probe rate for probe-based methods)

Outputs are written under `benchmarks/run_poll_rate_YYYYMMDD_HHMMSS/` and include
`poll_rate_*hz.csv` plus `poll_rate_summary.csv`.

See `bench/POLL_RATE_NOTES.md` for cross-method poll-rate sweep summaries.
`poll_rate_summary.csv` now includes mean/max poll_time_err_count columns when available.
When counts are available, poll_time_err p95 stats are computed only from rows
with nonzero counts.
Summary columns: `poll_rate_hz`, mean/max p95 AB/BA, mean/max count AB/BA, `rows`.
Additional summary columns include mean/max `overhead_bps` and mean/max `teleop_rms`.

Note: some scenarios (e.g., `E8_budget_200bps`, `E9_teleop`) can yield zero
poll_time_err metrics for baseline methods in peer_bench; teleop scenarios still
emit teleop RMS/max metrics in the CSV rows.
