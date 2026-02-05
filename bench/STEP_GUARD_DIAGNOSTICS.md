# StepGuard Gate Diagnostics

Goal: identify which StepGuard gates block step recovery.

## Run

```
SEEDS=2 GRID=1 THREADS=$(nproc) bench/run_stepguard_sweep.sh
python3 bench/report_stepguard_diag.py benchmarks/run_stepguard_20260203_072706/peer_bench.csv \
  --out benchmarks/run_stepguard_20260203_072706/stepguard_diag.md
```

Outputs:
- `benchmarks/run_stepguard_20260203_072706/peer_bench.csv`
- `benchmarks/run_stepguard_20260203_072706/stepguard_diag.md`

## Findings (gate ratios)

From `stepguard_diag.md` (includes iqr5/iqr10 dynamic threshold variants):

- **no_near gate** is ~0.0 across E0/E4/E5/E6 and all clock‑step scenarios (E14–E21).
- **rtt_guard_ok** is also ~0.0 on clock‑step scenarios, so StepGuard is filtered before innovation checks.
- **stable/enough** are mostly near 1.0, so they are not the bottleneck.

### Interpretation

StepGuard rarely gets past the early gates (no_near and rtt_guard), so innovation thresholding never fires. This explains the lack of step recovery and suggests that gate thresholds (near‑floor and RTT guard) are overly strict under step conditions.

Next tuning targets:
- Relax or conditionally bypass no_near / RTT guard during step detection windows.
- Make no_near use p10 rather than p0, or use a guard band tied to IQR.
- Consider a step‑specific RTT guard (looser than E6 path‑change guard).

## Quick gate check (E27/E28)

Run:
- `benchmarks/run_stepguard_20260203_083936/stepguard_diag.md`
- `benchmarks/run_stepguard_20260203_084258/stepguard_diag.md`
- `benchmarks/run_stepguard_20260203_084721/stepguard_diag.md`
- `benchmarks/run_stepguard_20260203_084915/stepguard_diag.md`
- `benchmarks/run_stepguard_20260203_085932/stepguard_diag.md`

Notes:
- Includes `nonear_p10`, `rtt_step`, `near_ratio`, and `single_dir` variants (quick scenarios only).
