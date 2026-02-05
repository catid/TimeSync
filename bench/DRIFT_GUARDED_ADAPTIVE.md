# Drift-Guarded Adaptive Design

Goal: keep baseline behavior on E0–E6 while retaining Adaptive’s E7 drift performance.

## Idea

Only shrink the TimeSync drift window when there is **consistent, low‑noise drift**:

- **Consistency:** multiple consecutive offset deltas with the same sign and above a trigger threshold.
- **Low noise:** variance of recent offset deltas below a threshold.
- **Hold:** once shrink happens, hold the small window for a limited time, then recover.

This avoids reacting to single outliers (E4 loss bursts / E5 reorder+dup), while still responding to sustained drift (E7).

## Proposed logic (per packet)

Inputs:
- `offset_sample` (from TimeSync)
- `trigger_us` (min delta magnitude)
- `window_size` (delta history size)
- `var_threshold_us2` (variance gate)
- `sign_count` (consecutive same‑sign requirement)
- `hold_us` (min time to hold short window)

Pseudo:

1. Compute `delta = offset_sample - last_offset`.
2. Update a ring of `delta` magnitudes; compute variance.
3. Track consecutive sign count (positive or negative) when `abs(delta) >= trigger_us`.
4. If variance <= threshold AND sign_count >= N, shrink drift window to min.
5. Hold short window for `hold_us`, then restore.

## Why it helps

- E4/E5 regressions are dominated by isolated spikes; the **sign count + variance gate** prevents shrink on outliers.
- E7 drift produces sustained, consistent deltas with low variance.

## Metrics to validate

- **E4/E5:** p95 should remain near baseline.
- **E7:** p95 should remain near Adaptive/VarGate.
- **E0/E1:** noise floor and stationary jitter should match baseline.
