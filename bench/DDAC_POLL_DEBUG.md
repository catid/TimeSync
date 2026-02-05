# DDAC poll validity debug

## Symptom
- `M4_TimeSyncDDAC:step` produced `no_poll` in short (3s) acceptance runs.

## Root cause
- DDAC only updates effective minima on the short‑window tick (2s). The first RTT‑guard update at 2s often runs before peer DDAC stats arrive; the next update is at 4s. With 3s runs, `algo_offset_valid` never gets set, so poll metrics stay empty.

## Fix
- After `UpdateDDAC()` on the RTT‑guard tick, call `UpdateAlgoOffset()` to populate `algo_offset_valid` immediately once effective mins are set.

## Validation
- With duration 6s, `M4_TimeSyncDDAC:step` now reports poll counts (`poll_time_err_valid_ab/ba=1`).
