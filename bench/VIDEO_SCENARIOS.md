# Video-Link Scenario Notes (E10–E21)

These scenarios model common Internet video-link behaviors with 20–120ms baseline latency. They are defined in `tests/peer_bench.cpp` under `BuildScenarios()`.

## E10_video_baseline
- Baseline 60ms one-way delay (AB/BA)
- Lognormal jitter (σ=0.6) with 5ms scale
- Purpose: typical stable link with heavy-tailed jitter

## E11_video_congestion
- Baseline 50ms
- 3ms uniform jitter
- Sinusoidal queueing wave: 50ms amplitude, 2s period
- Purpose: congestion wave / buffer oscillation

## E12_video_bursty_loss
- Baseline 80ms
- 5ms uniform jitter
- Delay spikes: 0.2ms at 0.2% probability
- Bursty loss: start prob 0.02, burst length 5–20
- Purpose: mobile/ISP bursts + transient queue spikes

## E13_video_path_change
- Baseline 40ms
- 3ms uniform jitter
- Step up at 20s (+40ms), partial recovery at 50s (-20ms)
- Purpose: handover / path change with partial recovery

## E23_asym_path_change
- Baseline 40ms
- 3ms uniform jitter
- Asymmetric step on AB only: +50ms at 20s, -20ms at 50s
- Reorder burst on AB: 5% reorder, 5–15ms reordering
- Purpose: asymmetric handover + reorder burst in one direction

## E24_asym_bufferbloat
- Baseline 40ms
- AB: lognormal jitter (σ=0.8) with 6ms scale
- AB queueing wave: 60ms amplitude, 3s period
- AB spikes: +150ms at 0.1% probability
- BA: 3ms uniform jitter, no queueing
- Purpose: asymmetric bufferbloat / uplink congestion

## E35_video_congestion_burst
- Baseline 70ms
- 4ms jitter
- Queueing wave: 80ms amplitude, 3s period
- Random-walk delay drift: ±2ms steps every 5s
- Spikes: +200ms at 0.1% probability
- Purpose: congestion bursts + slow delay walk in 20–120ms range

## E36_video_handover_burst
- Baseline 40ms
- 3ms jitter
- Step up at 25s (+60ms)
- Bursty loss: start prob 0.02, burst length 5–15
- Purpose: handover + burst loss events on a video link

## E40_video_highlat_reorder
- Baseline 120ms
- 6ms jitter
- Queueing wave: 80ms amplitude, 3s period
- Reorder: 3% with 5–15ms delays (both directions)
- Purpose: high‑latency link with congestion + reordering

## E71_video_latency_ramp
- Baseline 20ms
- 2ms jitter
- Random‑walk delay (0–100ms cap, 4ms steps every 2s)
- Queueing pulse: 20ms amplitude, 3s period
- Purpose: realistic 20–120ms RTT drift with periodic queues

## E72_video_congestion_ramp
- Baseline 40ms
- 3ms jitter
- Linear ramp +1ms/s (floor rises ~90ms over 90s)
- Queueing pulse: 30ms amplitude, 5s period
- Purpose: rising congestion floor with periodic queueing

## E73_video_jitter_burst
- Baseline 60ms
- Lognormal jitter (σ=0.9) with 5ms scale
- Spikes: +120ms at 0.3% probability
- Purpose: bursty jitter/spikes on a video link

## E74_video_path_bloat
- Baseline 50ms
- 4ms jitter
- Step up at 30s (+80ms) both directions
- Queueing wave: 80ms amplitude, 2s period
- Purpose: path change plus bufferbloat burst

## E75_video_asym_path_bloat
- Baseline 40ms
- 3ms jitter
- Asymmetric path step at 25s (+100ms AB, +20ms BA)
- Queueing on AB: 90ms amplitude, 2.5s period
- Purpose: asymmetric path change under congestion

## E76_video_queue_sawtooth
- Baseline 60ms
- 5ms jitter
- Sawtooth queue: 120ms amplitude, 4s period
- Spikes: +150ms at 0.2% probability
- Purpose: persistent queueing with periodic drain

## E77_video_asym_loss_burst
- Baseline 30ms
- 3ms jitter
- Queue wave: 70ms AB / 50ms BA, 8s/10s period
- Bursty loss: AB 3% (4–12), BA 1% (2–6)
- Reorder: AB 1%, BA 0.5% (5–15ms)
- Purpose: uplink‑heavy loss bursts under periodic congestion

## E78_video_path_flap_jitter
- Baseline 50ms
- 4ms jitter
- AB step at 20s (+70ms), step back at 45s (-50ms)
- BA step at 25s (+20ms), step back at 50s (-10ms)
- Random‑walk delay: AB ±6ms/3s (cap 60ms), BA ±4ms/4s (cap 40ms)
- Spikes: +100ms at 0.2% probability
- Purpose: asymmetric path flaps with jitter drift and spikes

## E80_video_congestion_pulses
- Baseline 20ms
- 3ms jitter
- Queue wave: 80ms amplitude, 4s period
- Spikes: +300ms at 0.3% probability
- Purpose: periodic congestion pulses with rare huge spikes

## E82_video_rtt_random_walk_cellular
- Baseline 20ms
- 4ms jitter
- Random‑walk delay: ±8ms/2s (cap 100ms)
- Bursty loss: AB 2% (3–8), BA 1.5% (2–6)
- Reorder: AB 1%, BA 0.5% (5–15ms)
- Purpose: cellular‑style RTT wander with burst loss

## E83_video_asym_bw_drop
- Baseline 30ms
- 3ms jitter
- Asymmetric step at 20s: +120ms AB, +40ms BA
- Partial recovery at 60s: -80ms AB, -30ms BA
- Purpose: asymmetric bandwidth drop with partial recovery

## E84_video_multipath_reorder_burst
- Baseline 40ms
- 4ms jitter
- Reorder: AB 5%, BA 3% (5–30ms)
- Dup: AB 1%, BA 0.5% (2ms delay)
- Spikes: +100ms at 0.2% probability
- Purpose: multipath‑style reorder/dup bursts with spikes

## E88_video_wifi_bursty
- Baseline 40ms
- 3ms Gaussian jitter (clipped at 3σ)
- Random‑walk delay: AB ±3ms/1.5s (cap 20ms), BA ±2.5ms/1.8s (cap 15ms)
- Bursty loss: AB 3% (3–10), BA 2% (2–8) + low base loss (0.5%/0.4%)
- Reorder: AB 2%, BA 1.5% (4–12ms)
- Purpose: WiFi‑like burst loss + correlated jitter + reordering

## E89_video_satlink_jitter
- Baseline 120ms
- Heavy‑tail Pareto jitter (α=1.6, 5ms scale)
- Spikes: +300ms at 0.2% probability
- Purpose: satellite‑link high latency with heavy‑tail jitter + spike bursts

## E14_clock_step_small
- Baseline 60ms
- Clock step on B: +2s at 20s
- Purpose: small but significant clock correction

## E15_clock_step_large
- Baseline 60ms
- Clock step on B: +30s at 30s
- Purpose: large discontinuity (e.g., NTP slew/step, suspend/resume)

## E16_clock_step_small_a_fwd
- Baseline 60ms
- Clock step on A: +2s at 20s
- Purpose: small forward step on the opposite endpoint

## E17_clock_step_small_a_back
- Baseline 60ms
- Clock step on A: -2s at 20s
- Purpose: small backward correction on the opposite endpoint

## E18_clock_step_large_a_fwd
- Baseline 60ms
- Clock step on A: +30s at 30s
- Purpose: large forward discontinuity on the opposite endpoint

## E19_clock_step_large_a_back
- Baseline 60ms
- Clock step on A: -30s at 30s
- Purpose: large backward discontinuity on the opposite endpoint

## E20_clock_step_small_b_back
- Baseline 60ms
- Clock step on B: -2s at 20s
- Purpose: small backward correction on B

## E21_clock_step_large_b_back
- Baseline 60ms
- Clock step on B: -30s at 30s
- Purpose: large backward discontinuity on B

## E63_clock_step_1s_a_fwd
- Baseline 60ms
- Clock step on A: +1s at 20s
- Purpose: small (seconds-scale) forward step on A

## E64_clock_step_1s_a_back
- Baseline 60ms
- Clock step on A: -1s at 20s
- Purpose: small (seconds-scale) backward step on A

## E65_clock_step_1s_b_fwd
- Baseline 60ms
- Clock step on B: +1s at 20s
- Purpose: small (seconds-scale) forward step on B

## E66_clock_step_1s_b_back
- Baseline 60ms
- Clock step on B: -1s at 20s
- Purpose: small (seconds-scale) backward step on B

## E67_clock_step_120s_a_fwd
- Baseline 60ms
- Clock step on A: +120s at 30s
- Purpose: huge forward discontinuity on A

## E68_clock_step_120s_a_back
- Baseline 60ms
- Clock step on A: -120s at 30s
- Purpose: huge backward discontinuity on A

## E69_clock_step_120s_b_fwd
- Baseline 60ms
- Clock step on B: +120s at 30s
- Purpose: huge forward discontinuity on B

## E70_clock_step_120s_b_back
- Baseline 60ms
- Clock step on B: -120s at 30s
- Purpose: huge backward discontinuity on B

## E22_video_jitter_pulse
- Baseline 30ms one-way
- Lognormal jitter (σ=0.8) with 4ms scale
- Sinusoidal queueing pulse: 30ms amplitude, 3s period
- Random-walk delay drift (±15ms cap, 2ms steps every 5s)
- Occasional spikes: +80ms at 0.3% probability
- Purpose: realistic 20–120ms RTT variation with congestion pulses and drift

## Quick validation scenarios (fast sanity checks)

## E27_clock_step_quick
- Duration 0.2s (min-delta interval 100ms)
- Send rate 50 Hz, poll 10 Hz
- Baseline 60ms
- Clock step on B: +2s at 100ms
- Purpose: fast step-guard/probe sanity check

## E28_noise_floor_quick
- Duration 0.2s (min-delta interval 100ms)
- Send rate 50 Hz, poll 10 Hz
- Baseline 2ms, no jitter, 50µs recv noise
- Purpose: fast baseline noise-floor check

## E29_asym_loss_reorder
- Baseline 20ms, 2ms jitter
- Loss burst + reordering/duplication on AB only
- Purpose: asymmetric loss/reorder stress without symmetric effects

## E32_asym_loss_reorder_ba
- Baseline 20ms, 2ms jitter
- Loss burst + reordering/duplication on BA only
- Purpose: asymmetric loss/reorder stress in the opposite direction

## E33_lowrate_step
- Baseline 60ms, 3ms jitter
- Low data rate: 2 Hz send, 1 Hz poll
- Clock step on B: +2s at 20s
- Purpose: probe-only step detection under sparse traffic

## E39_lowrate_jitter_step
- Baseline 60ms, 8ms jitter
- Low data rate: 2 Hz send, 1 Hz poll
- Clock step on B: +2s at 20s
- Purpose: sparse traffic with heavier jitter

## E41_lowrate_randstep
- Baseline 60ms, 4ms jitter
- Low data rate: 2 Hz send, 1 Hz poll
- Random step time in [10s, 50s], step size 1–5s (B only)
- Purpose: randomized low‑rate clock steps

## E42_lowrate_loss_step
- Baseline 60ms, 4ms jitter
- Low data rate: 2 Hz send, 1 Hz poll
- AB burst loss: start prob 0.02, burst length 3–8
- Clock step on B: +2s at 20s
- Purpose: sparse traffic with burst loss + step

## E43_lowrate_asym_reorder_step
- Baseline 60ms, 4ms jitter
- Low data rate: 2 Hz send, 1 Hz poll
- AB reorder: 5% with 5–15ms delays
- Clock step on B: +2s at 20s
- Purpose: sparse traffic with asymmetric reorder + step

## E44_lowrate_probe_burst
- Baseline 60ms, 4ms jitter
- Low data rate: 2 Hz send, 10 Hz poll
- Clock step on B: +2s at 20s
- Purpose: emulate probe bursts via higher poll rate

## E45_clock_jump_small_a_fwd
- Baseline 60ms, 5ms jitter
- Clock jump on A: +2s at 20s
- Purpose: small forward clock jump on sender side

## E45_clock_jump_small_a_back
- Baseline 60ms, 5ms jitter
- Clock jump on A: -2s at 20s
- Purpose: small backward clock jump on sender side

## E45_clock_jump_large_b_fwd
- Baseline 60ms, 5ms jitter
- Clock jump on B: +120s at 20s
- Purpose: large forward clock jump on receiver side

## E45_clock_jump_large_b_back
- Baseline 60ms, 5ms jitter
- Clock jump on B: -120s at 20s
- Purpose: large backward clock jump on receiver side

## E46_video_latency_congestion
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- AB/BA queue periods: 10s / 12s
- Step at 30s: +20ms AB, +10ms BA
- Jitter: 3ms
- Purpose: video-link latency sweep with congestion step

## E47_video_loss_reorder
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- AB/BA queue periods: 10s / 12s
- AB burst loss: start prob 0.02, length 3–8; BA burst loss: start prob 0.01, length 2–6
- AB reorder: 5% with 5–20ms delays
- Jitter: 3ms
- Purpose: video-link latency with burst loss + asymmetric reorder

## E48_video_congestion_step
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- AB/BA queue periods: 9s / 11s
- Congestion step at 15s: +20ms AB, +15ms BA
- Recovery step at 40s: -15ms AB, -10ms BA
- Jitter: 4ms
- Purpose: video-link latency with congestion step + recovery

## E49_video_lognormal_jitter
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- Lognormal jitter (sigma 0.7), 5ms scale
- AB/BA queue periods: 11s / 13s
- Purpose: heavy-tailed jitter under video latency sweep

## E50_video_step_pathchange
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- Congestion step at 25s: +30ms AB, -15ms BA (asymmetric path change)
- Recovery step at 45s: -20ms AB, +10ms BA
- Jitter: 4ms
- Purpose: video-link latency with congestion + asymmetric path change

## E51_video_drift_congestion
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- Congestion step at 30s: +20ms AB, +10ms BA
- Drift: A +50ppm, B -50ppm
- Jitter: 4ms
- Purpose: combined drift + congestion under video latency sweep

## E52_video_bimodal_pathchange
- Baseline 20ms with bimodal delay (80ms high mode AB, 60ms high mode BA)
- High‑mode probability: AB 25%, BA 20%
- Asymmetric step at 25s: +25ms AB, -15ms BA
- Jitter: 2ms
- Purpose: heavy‑tail bimodal delay + path asymmetry

## E53_randstep_normal
- Baseline 20ms, 2ms jitter
- Normal data rate: 60 Hz send, 1 Hz poll
- Random step time in [10s, 50s], size 1–5s
- Random side (A or B)
- Purpose: randomized step timing/side under normal traffic

## E54_drift_sign_flip
- Baseline 20ms, 2ms jitter
- Drift: A +50ppm, B -50ppm until 30s, then flip sign
- Purpose: stress skew tracking on drift sign reversal

## E59_path_flap
- Baseline 20ms, 3ms jitter
- AB: step up at 15s (+30ms), step down at 30s (-30ms), random-walk delay steps every 5s (±8ms, capped at 40ms)
- BA: step down at 20s (-15ms), step up at 40s (+15ms), random-walk delay steps every 6s (±6ms, capped at 30ms)
- Purpose: repeated path flaps with asymmetric delay shifts

## E55_video_latency_spike_train
- Baseline 20ms with queue sweep to 120ms (sinusoidal queue)
- AB/BA queue periods: 9s / 11s
- Spike train: 2% AB (+60ms), 1.5% BA (+50ms)
- Jitter: 3ms
- Purpose: periodic congestion with random spike bursts

## E56_video_asym_jitter_ramp
- Baseline 20ms
- AB Gaussian jitter 8ms (clipped at 3σ); BA Gaussian jitter 3ms (clipped at 3σ)
- Delay ramp: AB +1.5ms/s, BA +0.3ms/s
- Purpose: asymmetric jitter with rising latency floor

## E57_video_pareto_jitter
- Baseline 20ms
- Pareto heavy‑tail jitter (alpha 1.8 AB, 2.0 BA), 2ms scale
- Queue wave: 80ms amplitude, 10s/12s period
- Purpose: heavy‑tail jitter under video latency sweep

## E58_video_spike_loss_reorder
- Baseline 20ms
- Queue wave: 80ms amplitude, 9s/11s period
- Spike train: 2% AB (+60ms), 1.5% BA (+50ms)
- Bursty loss: AB 0.02 (3–8), BA 0.015 (2–6)
- Reorder: AB 4%, BA 2% (5–20ms)
- Purpose: combined spike congestion + loss/reorder bursts

## E60_video_correlated_jitter
- Baseline 20ms
- Queue wave: 60ms amplitude, 11s/13s period
- Random‑walk delay: AB steps ±4ms every 2s (cap 40ms), BA steps ±3.5ms every 2.5s (cap 35ms)
- Jitter: 3ms
- Purpose: correlated jitter / Wi‑Fi‑style congestion drift

## E61_video_congestion_120ms
- Baseline 20ms
- Queue wave: 80ms amplitude, 9s/11s period
- Congestion step at 20s: +100ms AB, +80ms BA (push toward 120ms)
- Partial recovery at 45s: -80ms AB, -60ms BA
- Jitter: 4ms
- Purpose: large congestion step to 120ms with recovery

## E62_video_asym_spike_step
- Baseline 20ms
- Queue wave: 80ms amplitude AB / 70ms BA, 9s/11s period
- Spike train: 2.5% AB (+70ms), 1% BA (+40ms)
- Congestion step at 25s: +40ms AB, +20ms BA
- Partial recovery at 45s: -30ms AB, -15ms BA
- Jitter: 3ms
- Purpose: asymmetric spikes + congestion step (realistic uplink/downlink mismatch)

## E94_video_link_realistic
- Baseline 20ms
- Queue wave: 80ms AB / 70ms BA, 9s/11s period
- Congestion step at 20s: +100ms AB, +80ms BA (20–120ms span)
- Partial recovery at 60s: -60ms AB, -40ms BA
- Bursty loss: AB 0.02 (2–6), BA 0.015 (2–5)
- Reorder: AB 3%, BA 1.5% (5–20ms)
- Purpose: mixed congestion, loss, reorder, and path‑change style dynamics

## E99_video_diurnal_rtt_pulses
- Baseline 30ms
- Diurnal RTT wave: 90ms amplitude, 120s period (30–120ms span)
- Jitter pulses: AB 3% (+50ms), BA 2% (+40ms)
- Purpose: slow RTT swing with intermittent jitter spikes

## E100_video_heavytail_reorder
- Baseline 30ms
- Pareto heavy‑tail jitter (alpha 1.6 AB / 1.8 BA, 3ms scale)
- Queue wave: 80ms AB / 70ms BA, 10s/12s period
- Reorder bursts: AB 5%, BA 3% (6–25ms)
- Bursty loss: AB 0.01 (2–5), BA 0.008 (2–4)
- Purpose: heavy‑tail queuing + reorder bursts
