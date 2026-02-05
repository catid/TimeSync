# StepGuard Probe Variants (E53/E45)

Run: `benchmarks/run_stepguard_probe_20260203_e53e45`
Script: `bench/run_stepguard_probe.sh` (SCENARIOS_OVERRIDE=E53/E45)

Methods:
- M4_TimeSync
- M4_TimeSyncStepGuard (+ probe/probe_lowrate/probe_relax/probe_only/probe_hard/probe_innov)

## Recovery summary (median across seeds)

E53_randstep_normal:
- M4_TimeSync: ~44s recovery at 1/5/10 ms thresholds
- All StepGuard probe variants: no recovery (step_recover = -1) even at 10 ms

E45 small jumps (A fwd/back):
- M4_TimeSync: ~7.6s / 8.6s recovery at 1/5/10 ms thresholds
- All StepGuard probe variants: no recovery at 10 ms

E45 large jumps (B fwd/back):
- No recovery for any method at 1/5/10 ms thresholds

## Takeaway
Probe-enabled StepGuard variants still fail to recover to 10 ms thresholds on E53/E45. Baseline recovers only for small jumps.
