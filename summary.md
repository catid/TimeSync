# TimeSync Benchmark Summary (core + skew overlay)

## Purpose
This document summarizes the current benchmark methodology, ranking approach, and results for the latest full core sweep, including the new sloped-window variants. It is intended to be **self‑contained** and reproducible.

**Latest run referenced here:**
- Core sweep CSV: `benchmarks/run_peer_20260204_040809/peer_bench.csv`
- Composite scorecard: `benchmarks/run_peer_20260204_040809/composite_scorecard_core.md`
- Skew‑only sweep CSV: `benchmarks/run_peer_skew_20260204_044950/peer_bench.csv`

---

## Testing methodology

### Simulation model
- Event‑driven Monte Carlo simulation (no wall‑clock time used).
- Two peers exchange data timestamps and 1 Hz MinDelta summaries.
- Each run simulates a specific scenario (loss, jitter, path change, congestion, etc.).
- Default poll rate: 10 Hz (per scenario). MinDelta exchange: 1 Hz.

### Baseline algorithm (M4_TimeSync)
The baseline algorithm is **M4_TimeSync**, a long‑window MinDelta estimator:

- Each packet provides a one‑way “delta” sample: `delta = t_recv_local − t_send_peer`.
- For each direction (C2S/S2C), keep the **minimum delta** over a long window (default ~60s).
- Each side sends its current MinDelta at **1 Hz**.
- Offset estimate uses the standard min‑delta formula:

```
offset_hat ≈ (C2S_min − S2C_min) / 2
```

This baseline is **very robust to positive outliers** (queueing, reordering, loss bursts) because it ignores large deltas, but it **does not model skew**—so it can lag under steady drift unless augmented by skew estimation or compensation.

### Seeds and run length
- Default seeds: **100**.
- Duration: scenario‑specific (most are ≥30s). Some scenarios override duration for stress or step tests.

### Drift overlay (core runs)
- A random, constant skew is applied **per run** in the range **±25–400 ppm**, unless the scenario already defines drift or skew.
- This overlay is enabled by default in `peer_bench` unless `--no-drift-overlay` is used.

### Metrics
All rankings below use the **poll‑time error p95** metric:

```
metric = max(poll_time_err_p95_ab_us, poll_time_err_p95_ba_us)
```

This captures the 95th‑percentile error in estimated remote time for both directions, and is the same metric used in the composite scorecard.

---

## How algorithms are ranked

### Composite score (core)
Computed by `bench/report_composite_score.py` on the core sweep CSV.

**Groups:**
- `E0-6` = E0–E6 scenarios (baseline noise, jitter, loss, reorder, etc.)
- `video` = all scenarios with `video` in the name
- `steps` = scenarios with `step`, `clock`, or `jump` in the name

**Weights:**
- E0‑6 = 3
- E7 = 2 (not required in core run)
- video = 1
- steps = 1

**Required groups:** `E0‑6`, `video`, `steps`

**Score:** weighted average of (group median / baseline group median), where baseline is **M4_TimeSync**.
Lower score is better.

### Baseline‑safe
A method is *baseline‑safe* if, for **every E0‑E6 scenario**, its median poll_p95 does **not exceed** the baseline (M4_TimeSync) by more than the configured tolerance (default 0%).

---

## Top 15 performing algorithms (overall)
(From `composite_scorecard_core.md`, overall list)

1. **M4_TimeSyncQuantile:q01_near_reset_shrink**
2. **M4_TimeSyncQuantile:q01_near_reset_tilted**
3. **M4_TimeSyncQuantile:q01_near_reset_q05**
4. **M4_TimeSyncQuantile:q01_near_reset_cse**
5. **M4_TimeSyncQuantile:q01_near_reset_cse_blend**
6. **M4_TimeSyncQuantile:q01_near_reset**
7. **M4_TimeSyncQuantile:q01_near_reset_shrink_xor**
8. **M4_TimeSyncQuantile:q01_near_reset_xor**
9. **M4_TimeSyncQuantile:q01_near_reset_timegate**
10. **M4_TimeSyncQuantile:q01_cse**
11. **M4_TimeSyncQuantile:q01_cse_blend**
12. **M4_TimeSyncQuantile:q05**
13. **M4_TimeSyncQuantileNearHit:qnh_short1s**
14. **M4_TimeSyncQuantile:q01_near_reset_shrink_rtt**
15. **M4_TimeSyncQuantile:q01_near_reset_rttguard**

### How the top algorithms work (family summary)
All top‑15 are **quantile‑based TimeSync variants**. They modify the baseline min‑window estimator to reduce “sticky min” behavior under drift, while keeping robustness to jitter.

#### Common core (TimeSyncQuantile)
- Replace strict min with a **low quantile** (p01 or p05) over the long window.
- Offset estimate uses `(C2S_quantile − S2C_quantile)/2`.
- More responsive to rising floors than strict min, but more sensitive to jitter.

#### Key variant mechanics
- **near_reset**: if near‑floor hits disappear for N consecutive windows, the long‑window floor is **reset upward**.
- **shrink**: temporarily **shrinks** the long window after a reset (fast reacquisition).
- **xor**: use anti‑symmetric (clock‑like) staleness detection to avoid reacting to symmetric network events.
- **rttguard**: suppress promotion/reset if RTT‑sum looks unstable.
- **cse / cse_blend**: use anti‑symmetric slope (coupled skew estimator) as an additional drift cue.
- **tilted**: sloped‑floor correction for drift (tilted min).
- **timegate**: only allow resets after a minimum age.
- **q05**: higher quantile (less aggressive) used as a baseline‑safe variant.

---

## Where the top algorithms perform poorly (and likely why)
The **top‑15 overall methods** share the same failure modes because they all use **aggressive low quantiles** plus reset/shrink logic.

### Worst scenarios (most common across top‑15)
These scenarios consistently show the **largest gap vs best method**:

- **E50_video_step_pathchange**
- **E76_video_queue_sawtooth**
- **E51_video_drift_congestion**
- **E49_video_lognormal_jitter**

### Likely causes
- **Aggressive low‑quantile + reset** is excellent at tracking a rising floor, but it **overreacts** in heavy queue + path‑change cases.
- **Step path changes** can look like drift and trigger resets/shrinks that raise error.
- **Sawtooth queueing** creates oscillating floors; aggressive reset/shrink can lock onto transient minima and destabilize.

**Example (worst gaps):**
- `M4_TimeSyncQuantile:q01_near_reset_shrink` on **E50_video_step_pathchange**: ~54 ms vs best ~8.5 ms (+532%)
- `M4_TimeSyncQuantile:q01_near_reset_shrink` on **E76_video_queue_sawtooth**: ~43 ms vs best ~3.5 ms (+1113%)

---

## Terse summary of other algorithm families (lower‑ranked)

- **Baseline / Hybrid / Sloped** (`M4_TimeSync`, `M4_TimeSyncHybrid:*`, `M4_TimeSyncSloped:*`)
  - Strong robustness on E0–E6; generally baseline‑safe.
  - Weaker on skew unless skew is explicitly modeled.

- **Skew‑aware estimators** (`M4_TimeSyncSkew`, `SkewReg`, `PerDirSkew`, `DualWindow`)
  - Better on skew‑only runs, but can regress in jittery scenarios.

- **Tilted / AgeComp / EnvelopeDecay**
  - Explicit drift compensation; strong on skew‑only. Needs gating to avoid overreaction.

- **DD‑AC / DD‑ACBlend**
  - Anti‑symmetric skew estimate + age compensation; good skew recovery, slightly more complex.

- **Adaptive / VarGate / Hysteresis / CUSUM**
  - Trigger‑based window changes. Can help in drift, but fragile under bursty jitter.

- **Consensus / MinReg / Shadow / MoE / KMin / KBest**
  - Alternative robust estimators. Performance depends heavily on thresholds and scenario mix.

- **Probe‑based** (`TimeSyncProbe`)
  - Uses explicit probes for skew; overhead tradeoff and mixed results.

---

## How to reproduce

### Build
```
cmake -S . -B build
cmake --build build --target peer_bench
```

### Full core sweep (100 seeds, default drift overlay ±25–400 ppm)
```
RUN_DIR="benchmarks/run_peer_$(date +%Y%m%d_%H%M%S)"
./build/peer_bench --threads 64 --out "$RUN_DIR/peer_bench.csv"
python3 bench/report_composite_score.py "$RUN_DIR" \
  --out "$RUN_DIR/composite_scorecard_core.md" \
  --require-groups E0-6,video,steps --top 15
```

### Skew‑only sweep
```
RUN_DIR="benchmarks/run_peer_skew_$(date +%Y%m%d_%H%M%S)"
./build/peer_bench --threads 64 --skew --out "$RUN_DIR/peer_bench.csv"
```

### Notes
- Stress scenarios are excluded from core by default; use `--include-disabled` to include them.
- The default drift overlay is enabled unless `--no-drift-overlay` is passed.
- All results in this summary were generated with **default seeds=100**.

---

## Appendix: Why baseline still ranks high
The core scorecard weights E0–E6 more heavily and **does not require E7**. The baseline min‑window estimator is extremely robust to jitter and outliers, so it remains near‑optimal on many core tests. Skew‑correcting algorithms generally improve drift but risk regressions in bursty queueing, which costs more under the current weights.
