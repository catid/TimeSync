# Peer Benchmark Full-Grid Context (2026-01-30)

## Goal
Run the **full grid sweep** for the peer-sync benchmark (M1–M4, estimator/discipline/sampling ablations), then generate a **complete PDF report** from a consolidated CSV.

## Current Status
Single-shot full-grid runs timed out (20–60+ minutes). The sweep was split into method-scoped chunks to make progress.

Completed chunks (DURATION=0.5s, SEEDS=1, GRID=1):
- `benchmarks/run_peer_fullgrid_chunked_20260130_180000/peer_bench_M1.csv`
- `benchmarks/run_peer_fullgrid_chunked_20260130_180000/peer_bench_M2.csv`
- `benchmarks/run_peer_fullgrid_chunked_20260130_180000/peer_bench_M3.csv`
- `benchmarks/run_peer_fullgrid_chunked_20260130_180000/peer_bench_M4_timesync.csv`

**Pending**:
- Piggyback grid chunk (M4 Piggyback variants). The attempt was interrupted.
- Merge all CSVs into a single `peer_bench.csv`.
- Generate final report PDF (`bench/report_peer_bench.py`).

## Commands Used (that worked)
```bash
# Chunked full-grid by method (fast-ish)
OUT_DIR=benchmarks/run_peer_fullgrid_chunked_20260130_180000

/home/catid/timesync/TimeSync/build/peer_bench --seeds 1 --duration 0.5 --grid --method M1 --threads "$(nproc)" --out "$OUT_DIR/peer_bench_M1.csv"
/home/catid/timesync/TimeSync/build/peer_bench --seeds 1 --duration 0.5 --grid --method M2 --threads "$(nproc)" --out "$OUT_DIR/peer_bench_M2.csv"
/home/catid/timesync/TimeSync/build/peer_bench --seeds 1 --duration 0.5 --grid --method M3 --threads "$(nproc)" --out "$OUT_DIR/peer_bench_M3.csv"
/home/catid/timesync/TimeSync/build/peer_bench --seeds 1 --duration 0.5 --grid --method TimeSync --threads "$(nproc)" --out "$OUT_DIR/peer_bench_M4_timesync.csv"
```

## Attempted (timed out)
```bash
# Single-shot full grid (timed out)
GRID=1 THREADS="$(nproc)" ./bench/run_peer_bench.sh

# Full grid w/ duration override (timed out)
GRID=1 SEEDS=1 DURATION=1 THREADS="$(nproc)" ./bench/run_peer_bench.sh benchmarks/run_peer_fullgrid_1s

# Scenario-only full grid (E0) still slow
/home/catid/timesync/TimeSync/build/peer_bench --seeds 1 --scenario E0 --duration 0.1 --grid --threads "$(nproc)" --out benchmarks/run_peer_fullgrid_chunked_20260130_180000/peer_bench_E0.csv
```

## Next Steps (to finish full grid report)
1. Run **Piggyback** grid chunk (may need shorter duration for speed):
```bash
OUT_DIR=benchmarks/run_peer_fullgrid_chunked_20260130_180000
/home/catid/timesync/TimeSync/build/peer_bench --seeds 1 --duration 0.5 --grid --method Piggyback --threads "$(nproc)" --out "$OUT_DIR/peer_bench_M4_piggy.csv"
```
If it times out, reduce duration to 0.2–0.3s and/or lower threads.

2. Merge CSVs into a single file:
```bash
OUT_DIR=benchmarks/run_peer_fullgrid_chunked_20260130_180000
{
  head -n 1 "$OUT_DIR/peer_bench_M1.csv"
  tail -n +2 -q "$OUT_DIR"/peer_bench_M*.csv
} > "$OUT_DIR/peer_bench.csv"
```

3. Generate the full PDF report:
```bash
bench/.venv/bin/python bench/report_peer_bench.py "$OUT_DIR/peer_bench.csv" "$OUT_DIR/peer_bench_report.pdf"
```

## Files Added for Peer Benchmark
- `tests/peer_bench.cpp`
- `bench/run_peer_bench.sh`
- `bench/report_peer_bench.py`
- `bench/README.md` (updated with peer bench usage)

## Notes
- Runtime is dominated by grid size × scenario count × duration. The grid expands quickly; splitting by method works.
- Full-duration (30s) grid will likely require multi-hour batch runs.
