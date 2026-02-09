# Component Ablation: R14 -> Simple M4 Variants (2026-02-09)

Source run:
- `benchmarks/run_component_ablation_20260209_054021/peer_bench.csv`
- 64 scenarios x 30 seeds
- Methods: `M4_TimeSync`, `r14_comp00..r14_comp07`, and two promoted references

Metric conventions:
- Per scenario metric = median over seeds of `max(poll_time_err_{pXX}_ab_us, poll_time_err_{pXX}_ba_us)`
- Aggregate score = geometric mean ratio vs `M4_TimeSync` across all 64 scenarios
- Lower is better

## Aggregate Results (vs M4_TimeSync)

| method | gm p50 ratio | gm p95 ratio | worst p50 regression | worst p95 regression |
| --- | ---: | ---: | --- | --- |
| `r14_comp07_irj_enable_bs19k_n12_age20s_sm50k_b45_c15k_iqrstreak_gfmx35` | 0.618 | 0.678 | 5.68x (`E100_video_heavytail_reorder`) | 2.62x (`E80_video_congestion_pulses`) |
| `r14_comp06_irj_enable_bs19k_n12_age20s_sm50k_b45_c15k_iqrstreak` | 0.619 | 0.678 | 5.68x (`E100_video_heavytail_reorder`) | 2.62x (`E80_video_congestion_pulses`) |
| `r14_comp05_irj_enable_bs19k_n12_age20s_sm50k_b45_c15k` | 0.619 | 0.679 | 5.68x (`E100_video_heavytail_reorder`) | 2.62x (`E80_video_congestion_pulses`) |
| `r14_comp04_irj_enable_bs19k_n12_age20s_sm50k` | 0.621 | 0.682 | 5.68x (`E100_video_heavytail_reorder`) | 2.62x (`E80_video_congestion_pulses`) |
| `r14_comp03_irj_enable_bs19k_n12` | 0.669 | 0.860 | 5.85x (`E100_video_heavytail_reorder`) | 4.70x (`E74_video_path_bloat`) |
| `r14_comp02_irj_enable_bs19k` | 0.786 | 1.160 | 18.47x (`E76_video_queue_sawtooth`) | 30.70x (`E76_video_queue_sawtooth`) |
| `r14_comp01_irj_enable` | 0.861 | 1.226 | 18.32x (`E76_video_queue_sawtooth`) | 30.26x (`E76_video_queue_sawtooth`) |
| `r14_comp00_base` | 0.619 | 0.678 | 5.68x (`E100_video_heavytail_reorder`) | 2.62x (`E80_video_congestion_pulses`) |
| `M4_TimeSync` | 1.000 | 1.000 | 1.00x | 1.00x |

Note: `r14_comp07` and `r14_irj_bs19k_n12_b45_c15k_age20s_sm50k_gfmx35` are numerically identical in this run.

## Key Scenarios (p50 us, ratio vs M4)

| method | E72 ramp | E40 reorder | E76 sawtooth | E82 rtt random walk | E108 mobile handover |
| --- | ---: | ---: | ---: | ---: | ---: |
| `M4_TimeSync` | 1042 (1.00x) | 483 (1.00x) | 600 (1.00x) | 6754 (1.00x) | 10195 (1.00x) |
| `r14_comp00_base` | 3727 (3.58x) | 409 (0.85x) | 889 (1.48x) | 4223 (0.63x) | 552 (0.05x) |
| `r14_comp01_irj_enable` | 764 (0.73x) | 6840 (14.16x) | 11004 (18.32x) | 8413 (1.25x) | 9936 (0.97x) |
| `r14_comp02_irj_enable_bs19k` | 2496 (2.40x) | 7238 (14.99x) | 11094 (18.47x) | 4501 (0.67x) | 9878 (0.97x) |
| `r14_comp03_irj_enable_bs19k_n12` | 3155 (3.03x) | 409 (0.85x) | 889 (1.48x) | 4413 (0.65x) | 9863 (0.97x) |
| `r14_comp04_irj_enable_bs19k_n12_age20s_sm50k` | 3155 (3.03x) | 409 (0.85x) | 889 (1.48x) | 4223 (0.63x) | 600 (0.06x) |
| `r14_comp05_irj_enable_bs19k_n12_age20s_sm50k_b45_c15k` | 3340 (3.21x) | 409 (0.85x) | 889 (1.48x) | 4223 (0.63x) | 563 (0.06x) |
| `r14_comp06_irj_enable_bs19k_n12_age20s_sm50k_b45_c15k_iqrstreak` | 3340 (3.21x) | 409 (0.85x) | 889 (1.48x) | 4223 (0.63x) | 563 (0.06x) |
| `r14_comp07_irj_enable_bs19k_n12_age20s_sm50k_b45_c15k_iqrstreak_gfmx35` | 3340 (3.21x) | 409 (0.85x) | 889 (1.48x) | 4223 (0.63x) | 552 (0.05x) |

## Incremental Effects (p50)

From one rung to the next (`better/worse/flat` scenarios):

- `comp00 -> comp01`: `28 / 36 / 0`; biggest improve `E113` 5.36x, biggest regress `E151` 31.19x
- `comp01 -> comp02`: `32 / 32 / 0`; biggest improve `E151` 33.00x, biggest regress `E113` 5.36x
- `comp02 -> comp03`: `31 / 5 / 28`; biggest improve `E40` 17.70x, biggest regress `E72` 1.26x
- `comp03 -> comp04`: `22 / 3 / 39`; biggest improve `E108` 16.44x, biggest regress `E36` 1.17x
- `comp04 -> comp05`: `3 / 2 / 59`; biggest improve `E78` 1.31x, biggest regress `E13` 1.06x
- `comp05 -> comp06`: `1 / 0 / 63`; max shift 1.03x
- `comp06 -> comp07`: `2 / 1 / 61`; max shift 1.02x

## Parameter-Invariance Readout

Late-stage variants are effectively invariant in this benchmark set:

- `comp05 vs comp06`: p50 ratio median 1.000x, p95 1.000x, max 1.029x
- `comp06 vs comp07`: p50 ratio median 1.000x, p95 1.000x, max 1.020x
- `comp07 vs promoted gfmx35 reference`: exactly identical across all scenarios

## Main Takeaways

- The two decisive stabilizers are:
  - `stale_streak_n = 12` (`comp03`) to remove huge E40/E76 failure modes from early IRJ forms.
  - `bilateral_min_age_us + bilateral_stale_max_us` (`comp04`) to recover `E108`.
- After `comp05`, additional knobs (`iqr streak add`, `guard fail max`) have very small impact.
- `E72` improves only modestly from `comp00` to `comp07` (3727 -> 3340 us, ~10%).
- The largest residual regressions are unrelated to late IRJ tuning and remain concentrated in heavy-tail/reorder/congestion cases (e.g., `E100`, `E47`, `E57`, `E58`, `E80`).
