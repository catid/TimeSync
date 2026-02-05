# Low-Rate Step Summary (E33/E39/E41/E42)

Scenarios:
- **E33_lowrate_step**: 2 Hz send, 1 Hz poll, 3ms jitter, +2s step on B.
- **E39_lowrate_jitter_step**: same, 8ms jitter.
- **E41_lowrate_randstep**: random step time (10–50s), size 1–5s.
- **E42_lowrate_loss_step**: burst loss + step.

Key findings (quick sweeps):
- StepGuard probe variants (probe, probe_lowrate, probe_relax, probe_only, probe_hard) do not improve recovery under low-rate traffic.
- Baseline M4_TimeSync still serves as reference; StepGuard resets rarely trigger with sparse data.

Artifacts:
- `bench/LOWRATE_STEP_SWEEP.md`
- `bench/STEP_ONLY_MULTI_SEED.md`
- `bench/STEP_GUARD_LOW_RATE_SWEEP.md`

Next ideas:
- Explicit probe-only step detector with a dedicated timer and threshold on probe offset deltas (bypass min-delta windows).
- Increase probe rate during detected step events (adaptive probing under a budget).
