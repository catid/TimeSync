# Extended Video Scenarios Summary

New scenarios added to broaden realism for 20–120ms links:
- **E35_video_congestion_burst**: 70ms baseline, congestion waves + random-walk delay drift + spikes.
- **E36_video_handover_burst**: 40ms baseline, step handover + bursty loss.
- **E40_video_highlat_reorder**: 120ms baseline, congestion waves + 3% reordering.
- **E46_video_latency_congestion**: 20–120ms sweep with a mid‑run congestion step.
- **E47_video_loss_reorder**: 20–120ms sweep with burst loss + asymmetric reorder.
- **E48_video_congestion_step**: 20–120ms sweep with congestion step + recovery.
- **E49_video_lognormal_jitter**: 20–120ms sweep with lognormal (heavy‑tailed) jitter.
- **E50_video_step_pathchange**: 20–120ms sweep with congestion step + asymmetric path change.
- **E51_video_drift_congestion**: 20–120ms sweep with drift + congestion step.
- **E52_video_bimodal_pathchange**: 20–120ms sweep with bimodal delay + asymmetric path change.
- **E55_video_latency_spike_train**: 20–120ms sweep with periodic congestion + random spike bursts.
- **E56_video_asym_jitter_ramp**: asymmetric Gaussian jitter with rising delay floor.
- **E57_video_pareto_jitter**: heavy‑tail Pareto jitter under queue wave.
- **E58_video_spike_loss_reorder**: spike train + burst loss + reorder bursts.
- **E59_path_flap**: repeated asymmetric delay shifts with random-walk drift.
- **E60_video_correlated_jitter**: correlated jitter / Wi‑Fi‑style random walk.
- **E61_video_congestion_120ms**: congestion step to 120ms with partial recovery.
- **E62_video_asym_spike_step**: asymmetric spike train + congestion step.
- **E71_video_latency_ramp**: 20–120ms random‑walk latency drift with periodic queue pulses.
- **E72_video_congestion_ramp**: linear congestion ramp (+1ms/s) plus periodic queues.
- **E73_video_jitter_burst**: lognormal jitter with rare +120ms spikes.
- **E74_video_path_bloat**: symmetric +80ms path step plus bufferbloat wave.
- **E75_video_asym_path_bloat**: asymmetric step (+100ms/+20ms) with congestion on AB.
- **E76_video_queue_sawtooth**: 120ms sawtooth queue with rare +150ms spikes.
- **E77_video_asym_loss_burst**: uplink‑heavy loss bursts under periodic congestion.
- **E78_video_path_flap_jitter**: asymmetric path flaps with jitter drift and spikes.
- **E80_video_congestion_pulses**: periodic congestion pulses with rare huge spikes.
- **E82_video_rtt_random_walk_cellular**: cellular‑style RTT wander with burst loss.
- **E83_video_asym_bw_drop**: asymmetric bandwidth drop with partial recovery.
- **E84_video_multipath_reorder_burst**: multipath‑style reorder/dup bursts with spikes.
- **E88_video_wifi_bursty**: Wi‑Fi‑like correlated jitter with burst loss + reorder.
- **E89_video_satlink_jitter**: 120ms baseline with heavy‑tail jitter + spike bursts.

Quick validation:
- Each scenario compiles and runs under `peer_bench` (1s sanity checks).

Latest sweep:
- `benchmarks/run_video_sweep_ext2_20260203_101005` (SEEDS=3, DURATION=3)
- Methods run via prefix match; baseline‑safe variants remain stable across E46–E48.

Latest sweep (extended):
- `benchmarks/run_video_sweep_20260203_ext5` (SEEDS=3, DURATION=3)
- Includes E55–E62; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, ramps + sawtooth):
- `benchmarks/run_video_sweep_20260203_ext6` (SEEDS=3, DURATION=3)
- Includes E71–E76; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, asym loss + path flap + pulses):
- `benchmarks/run_video_sweep_20260203_ext7` (SEEDS=3, DURATION=3)
- Includes E77–E80; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, cellular RTT RW):
- `benchmarks/run_video_sweep_20260203_ext8` (SEEDS=3, DURATION=3)
- Includes E82; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, asym bw drop):
- `benchmarks/run_video_sweep_20260203_ext9` (SEEDS=3, DURATION=3)
- Includes E83; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, multipath reorder):
- `benchmarks/run_video_sweep_20260203_ext10` (SEEDS=3, DURATION=3)
- Includes E84; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, WiFi bursty):
- `benchmarks/run_video_sweep_20260203_ext11b` (SEEDS=3, DURATION=3)
- Includes E88; baseline‑safe methods remain stable in the aggregated table.

Latest sweep (extended, satlink jitter):
- `benchmarks/run_video_sweep_20260203_ext12` (SEEDS=3, DURATION=3)
- Includes E89; baseline‑safe methods remain stable in the aggregated table.

Recommended usage:
- Include E35/E36/E40 in contender sweeps to ensure stability under long‑RTT congestion and reordering.
- Include E46–E48 in video sweeps to validate behavior under congestion steps and loss/reorder bursts.
- Compare baseline‑safe methods (TimeSync, Shadow variants) against drift‑capable variants to track regressions.
