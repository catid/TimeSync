# Low-rate step analysis (E33/E39/E41–E44)

## Sources
- `bench/LOWRATE_STEP_SWEEP.md`
- `bench/LOWRATE_INNOV_SWEEP.md`
- `bench/PROBE_MAD_SWEEP.md`

## Findings
- Baseline `M4_TimeSync` and `M4_TimeSyncStepGuard` remain equivalent under low-rate steps.
- Probe variants (`probe`, `probe_lowrate`, `probe_relax`, `probe_only`, `probe_hard`, `probe_innov`) show no measurable accuracy improvement across E33/E39/E41–E44 in recent sweeps.
- MAD threshold tuning (`probe_mad4/6/10`) does not change outcomes vs `probe_innov`.
- Probe methods add overhead (66 bps vs 18 bps baseline), so they regress fairness without accuracy gains.

## Implications
- Low-rate step recovery is still an open gap for baseline-safe methods.
- Further gains likely require new logic (e.g., directional age compensation or shadow promotion) rather than more probe gating/tuning.
