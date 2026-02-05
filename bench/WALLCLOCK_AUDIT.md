# Wallclock Usage Audit (2026-02-03)

This project’s simulations are event-driven; `steady_clock` is only used for calibration and progress display, not to advance simulated time.

## Findings

`tests/experiments.cpp` uses wallclock time only in these locations:

- `CalibrateThroughput` (`steady_clock` start/now/finish) for auto-scaling throughput calibration.
- Progress thread (`steady_clock` for ETA + `sleep_for(200ms)`) when `--progress` is enabled.

No simulation loops use wallclock time; they advance by scheduled event timestamps.
`tests/peer_bench.cpp` uses event-driven scheduling only.

## Batch mode

Use `--batch` (alias `--no-wallclock`) to disable auto-scaling and progress display. This avoids all wallclock usage during runs.

## Related flags

- `--no-progress`: disables progress thread (no `steady_clock` or sleep)
- `--auto`: enables throughput calibration (uses `steady_clock`)

## Source grep

```text
rg -n "steady_clock|sleep_for" tests bench src inc
```

## Automated audit

Run:

```bash
./bench/check_wallclock.sh
```

This script asserts wallclock references only exist in `tests/experiments.cpp` and the count matches the known calibration/progress uses.

Latest run (2026-02-03):

```
Wallclock audit OK (tests/experiments.cpp count=6).
```

## Non-simulation utilities

- Report scripts (e.g., `bench/report_peer_bench.py`) include `datetime.now()` only to timestamp output PDFs.
