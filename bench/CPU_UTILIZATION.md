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
