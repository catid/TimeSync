# Scenario Realism Classification (teleop across states)

Each scenario is manually classified as CORE (realistic) or STRESS (unrealistic for the target use-case).
The classification below is the source of truth for core vs stress.

| Scenario | Classification | Rationale |
|---|---|---|
| E0_noise_floor | CORE | Teleop‑relevant network condition |
| E1_stationary_jitter | CORE | Teleop‑relevant network condition |
| E2_asymmetry_40ms | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E3_bufferbloat | CORE | Teleop‑relevant network condition |
| E4_loss_burst | CORE | Teleop‑relevant network condition |
| E5_reorder_dup | CORE | Teleop‑relevant network condition |
| E80_ts24_poison | STRESS | Timestamp corruption / wrap stress |
| E29_asym_loss_reorder | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E32_asym_loss_reorder_ba | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E33_lowrate_step | STRESS | Clock discontinuity / step/jump/freeze |
| E39_lowrate_jitter_step | STRESS | Clock discontinuity / step/jump/freeze |
| E41_lowrate_randstep | STRESS | Clock discontinuity / step/jump/freeze |
| E42_lowrate_loss_step | STRESS | Clock discontinuity / step/jump/freeze |
| E43_lowrate_asym_reorder_step | STRESS | Clock discontinuity / step/jump/freeze |
| E44_lowrate_probe_burst | STRESS | Clock discontinuity / step/jump/freeze |
| E90_sparse_traffic | STRESS | Sparse traffic or extreme overhead budget |
| E91_clock_freeze_resume | STRESS | Clock discontinuity / step/jump/freeze |
| E92_clock_slew | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E93_asym_timestamp_noise | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E101_ts_quant_asym_noise | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E45_clock_jump_small_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E45_clock_jump_small_a_back | STRESS | Clock discontinuity / step/jump/freeze |
| E45_clock_jump_large_b_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E45_clock_jump_large_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E95_clock_jump_small_b_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E96_clock_jump_small_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E97_clock_jump_large_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E98_clock_jump_large_a_back | STRESS | Clock discontinuity / step/jump/freeze |
| E46_video_latency_congestion | CORE | Teleop‑relevant network condition |
| E47_video_loss_reorder | CORE | Teleop‑relevant network condition |
| E48_video_congestion_step | CORE | Teleop‑relevant network condition |
| E49_video_lognormal_jitter | CORE | Teleop‑relevant network condition |
| E50_video_step_pathchange | CORE | Teleop‑relevant network condition |
| E51_video_drift_congestion | CORE | Teleop‑relevant network condition |
| E52_video_bimodal_pathchange | CORE | Teleop‑relevant network condition |
| E53_randstep_normal | STRESS | Clock discontinuity / step/jump/freeze |
| E54_drift_sign_flip | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E122_skew_reset_zero | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E85_drift_rw_signflip | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E119_drift_rw_long | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E133_temp_drift_sine_step | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E120_route_rebind | CORE | Teleop‑relevant network condition |
| E135_nat_rebind_outage_flip | CORE | Teleop‑relevant network condition |
| E121_cpu_jitter_burst | CORE | Teleop‑relevant network condition |
| E129_asym_scheduler_noise_burst | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E86_clock_step_300s_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E87_clock_step_300s_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E55_video_latency_spike_train | CORE | Teleop‑relevant network condition |
| E56_video_asym_jitter_ramp | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E57_video_pareto_jitter | CORE | Teleop‑relevant network condition |
| E58_video_spike_loss_reorder | CORE | Teleop‑relevant network condition |
| E60_video_correlated_jitter | CORE | Teleop‑relevant network condition |
| E61_video_congestion_120ms | CORE | Teleop‑relevant network condition |
| E62_video_asym_spike_step | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E6_path_change | CORE | Teleop‑relevant network condition |
| E103_asym_path_switch | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E104_bursty_uplink_steady_downlink | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E105_intermittent_probe_rate | CORE | Teleop‑relevant network condition |
| E106_bidirectional_queue_coupling | CORE | Teleop‑relevant network condition |
| E107_temp_skew_sine | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E108_mobile_handover | CORE | Teleop‑relevant network condition |
| E114_skew_sign_flip | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E115_bursty_jitter_diurnal_skew | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E116_satellite_burst_loss_long_rtt | STRESS | Satellite / extreme RTT conditions |
| E134_satellite_doppler_skew | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E117_upstream_congestion_waves | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E131_congestion_wave_corr | CORE | Teleop‑relevant network condition |
| E118_cross_traffic_asym_jitter | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E132_uplink_saturation_ack_compress | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E109_asym_baseline_periodic_drops | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E110_jitter_square_wave | CORE | Teleop‑relevant network condition |
| E111_periodic_outage | CORE | Teleop‑relevant network condition |
| E127_asym_periodic_outage | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E112_oneway_priority_inversion | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E113_correlated_loss_jitter | CORE | Teleop‑relevant network condition |
| E94_drift_sym_pathchange | CORE | Teleop‑relevant network condition |
| E94_video_link_realistic | CORE | Teleop‑relevant network condition |
| E59_path_flap | CORE | Teleop‑relevant network condition |
| E99_video_diurnal_rtt_pulses | STRESS | Long‑period diurnal RTT pulses |
| E100_video_heavytail_reorder | CORE | Teleop‑relevant network condition |
| E7_drift | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E7_drift_25ppm | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E7_drift_100ppm | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E7_drift_200ppm | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E7_drift_300ppm | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E7_drift_400ppm | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E7_drift_ramp | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E34_asym_drift | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E37_asym_drift_step | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E38_asym_drift_netmix | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E8_budget_200bps | STRESS | Sparse traffic or extreme overhead budget |
| E9_teleop | CORE | Teleop‑relevant network condition |
| E10_video_baseline | CORE | Teleop‑relevant network condition |
| E11_video_congestion | CORE | Teleop‑relevant network condition |
| E12_video_bursty_loss | CORE | Teleop‑relevant network condition |
| E13_video_path_change | CORE | Teleop‑relevant network condition |
| E23_asym_path_change | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E24_asym_bufferbloat | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E35_video_congestion_burst | CORE | Teleop‑relevant network condition |
| E36_video_handover_burst | CORE | Teleop‑relevant network condition |
| E40_video_highlat_reorder | CORE | Teleop‑relevant network condition |
| E71_video_latency_ramp | CORE | Teleop‑relevant network condition |
| E72_video_congestion_ramp | CORE | Teleop‑relevant network condition |
| E73_video_jitter_burst | CORE | Teleop‑relevant network condition |
| E74_video_path_bloat | CORE | Teleop‑relevant network condition |
| E75_video_asym_path_bloat | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E76_video_queue_sawtooth | CORE | Teleop‑relevant network condition |
| E77_video_asym_loss_burst | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E78_video_path_flap_jitter | CORE | Teleop‑relevant network condition |
| E80_video_congestion_pulses | CORE | Teleop‑relevant network condition |
| E82_video_rtt_random_walk_cellular | CORE | Teleop‑relevant network condition |
| E124_cellular_rlc_bimodal | CORE | Teleop‑relevant network condition |
| E125_bufferbloat_ramp_drop | CORE | Teleop‑relevant network condition |
| E83_video_asym_bw_drop | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E84_video_multipath_reorder_burst | CORE | Teleop‑relevant network condition |
| E140_reorder_dup_storm | CORE | Teleop‑relevant network condition |
| E88_video_wifi_bursty | CORE | Teleop‑relevant network condition |
| E123_wifi_roam_asym_burst | STRESS | Asymmetric path or one‑way traffic (assumed symmetric) |
| E89_video_satlink_jitter | STRESS | Satellite / extreme RTT conditions |
| E126_video_heavy_tail_mix | CORE | Teleop‑relevant network condition |
| E128_ts24_wrap_stress | STRESS | Timestamp corruption / wrap stress |
| E130_heavytail_step_reorder | CORE | Teleop‑relevant network condition |
| E14_clock_step_small | STRESS | Clock discontinuity / step/jump/freeze |
| E15_clock_step_large | STRESS | Clock discontinuity / step/jump/freeze |
| E16_clock_step_small_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E136_clock_jump_matrix_pp | STRESS | Clock discontinuity / step/jump/freeze |
| E137_clock_jump_matrix_pn | STRESS | Clock discontinuity / step/jump/freeze |
| E138_clock_jump_matrix_np | STRESS | Clock discontinuity / step/jump/freeze |
| E139_clock_jump_matrix_nn | STRESS | Clock discontinuity / step/jump/freeze |
| E17_clock_step_small_a_back | STRESS | Clock discontinuity / step/jump/freeze |
| E18_clock_step_large_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E19_clock_step_large_a_back | STRESS | Clock discontinuity / step/jump/freeze |
| E20_clock_step_small_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E21_clock_step_large_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E63_clock_step_1s_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E64_clock_step_1s_a_back | STRESS | Clock discontinuity / step/jump/freeze |
| E65_clock_step_1s_b_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E66_clock_step_1s_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E67_clock_step_120s_a_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E68_clock_step_120s_a_back | STRESS | Clock discontinuity / step/jump/freeze |
| E69_clock_step_120s_b_fwd | STRESS | Clock discontinuity / step/jump/freeze |
| E70_clock_step_120s_b_back | STRESS | Clock discontinuity / step/jump/freeze |
| E27_clock_step_quick | STRESS | Clock discontinuity / step/jump/freeze |
| E28_noise_floor_quick | STRESS | Clock discontinuity / step/jump/freeze |
| E26_clock_step_mc | STRESS | Clock discontinuity / step/jump/freeze |
| E30_clock_step_mc_side | STRESS | Clock discontinuity / step/jump/freeze |
| E31_drift_step_mc | STRESS | Clock discontinuity / step/jump/freeze |
| E79_clock_step_mc_huge | STRESS | Clock discontinuity / step/jump/freeze |
| E81_clock_skew_random_walk_mc | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E102_clock_skew_random_walk | STRESS | Time‑varying skew dynamics (sign flips/ramps/random walk) |
| E22_video_jitter_pulse | CORE | Teleop‑relevant network condition |
