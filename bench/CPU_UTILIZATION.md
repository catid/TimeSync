# CPU Utilization Notes

## Monte Carlo (experiments)

Sampled CPU usage with 128 threads using a long Monte Carlo run:

- Command: `./build/experiments --montecarlo 300000 --threads 128 --batch --no-progress --csv benchmarks/run_mc_cpu_20260131_000620/montecarlo.csv`
- Snapshot: `benchmarks/run_mc_cpu_20260131_000620/cpu_snapshot.txt`
- Observations:
  - Threads: 129
  - Total CPU: ~3725–3924% across samples (ps -L sum)
  - Active threads: 129

## Peer bench

Sampled CPU usage with 128 threads using a heavier grid run:

- Command: `./build/peer_bench --seeds 400 --threads 128 --grid --duration 2 --out benchmarks/run_peer_cpu_20260131_001120/peer_bench.csv`
- Snapshot: `benchmarks/run_peer_cpu_20260131_001120/cpu_snapshot.txt`
- Observations:
  - Threads: 129
  - Total CPU: ~1654–2033% across samples (ps -L sum)
  - Active threads: 129

Notes:
- CPU percentage is the sum of per-thread %CPU from `ps -L`.
- Both runs show the process spinning up 129 threads, with high aggregate CPU utilization.

## 2026-02-02: Thread utilization check (64 threads)

Measured with `/usr/bin/time` and verified system limits:

- Command (short): `./build/peer_bench --scenario E7_drift --grid --seeds 10 --threads 64 --duration 10 --out /tmp/peer_util.csv`
  - CPU%: ~1167% (≈11.7 cores)
- Command (long): `./build/peer_bench --scenario E7_drift --grid --seeds 100 --threads 64 --duration 30 --out /tmp/peer_util_long.csv`
  - CPU%: ~1260% (≈12.6 cores)

Environment checks:
- cgroup quota: `cpu.max` = `max 100000` (no quota)
- cpuset: `0-127` (full affinity)
- process affinity: `taskset -pc $$` => `0-127`

Interpretation: no OS/cgroup CPU cap detected; the lower CPU% likely reflects workload granularity or scheduling overhead rather than a hard limit.
