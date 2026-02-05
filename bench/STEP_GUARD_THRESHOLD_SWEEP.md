# StepGuard Threshold Sweep (E53/E45)

Run: `bench/run_stepguard_threshold_sweep.sh benchmarks/run_stepguard_threshold_20260203`

Scenarios:
- E53_randstep_normal
- E45_clock_jump_small_a_fwd
- E45_clock_jump_small_a_back
- E45_clock_jump_large_b_fwd
- E45_clock_jump_large_b_back

Methods:
- M4_TimeSync (baseline)
- M4_TimeSyncStepGuard (+ variants: th0p5ms, th2ms, iqr5, iqr10, gn_iqr, nonear_p10)

## Summary
- All StepGuard threshold/gn_iqr variants produced identical recovery metrics in this sweep.
- StepGuard variants did **not** recover to the 1 ms threshold in E53/E45 (step_recover metrics remain -1 in the CSV), so the sweep did not show improvement for these cases.
- Baseline M4_TimeSync eventually recovers for the small jumps and random-step case, but not for the large B jumps.

## Baseline recovery times (median across seeds)
- E53_randstep_normal: step_recover ≈ 43s, step_recover3 ≈ 45s
- E45_clock_jump_small_a_fwd: step_recover ≈ 7.8s, step_recover3 ≈ 8.0s
- E45_clock_jump_small_a_back: step_recover ≈ 8.1s, step_recover3 ≈ 8.3s
- E45_clock_jump_large_b_fwd: no recovery within threshold
- E45_clock_jump_large_b_back: no recovery within threshold

## Next directions
- Consider probe-based StepGuard variants (probe_*), or relaxing the recovery threshold used in metrics, if we want to measure partial recovery for large jumps.
- Alternatively, revisit StepGuard logic (near-hit gating / step thresholds) for large remote jumps.

---

# StepGuard Threshold Sweep (E63–E70)

Run: `bench/run_stepguard_threshold_sweep.sh benchmarks/run_stepguard_threshold_e63e70_20260203`

Scenarios:
- E63_clock_step_1s_a_fwd
- E64_clock_step_1s_a_back
- E65_clock_step_1s_b_fwd
- E66_clock_step_1s_b_back
- E67_clock_step_120s_a_fwd
- E68_clock_step_120s_a_back
- E69_clock_step_120s_b_fwd
- E70_clock_step_120s_b_back

Summary:
- All StepGuard threshold/gn_iqr variants were identical in this sweep.
- E63/E66 show StepGuard p95 ≈ 8 µs (direction‑specific), but E64/E65 remain ~500k µs, so overall step recovery is inconsistent.
- Large 120s steps remain huge (≈ 99–127s p95); no improvement vs baseline.
- StepGuard variants are **not** baseline‑safe (within_baseline_E0_6 = no).
