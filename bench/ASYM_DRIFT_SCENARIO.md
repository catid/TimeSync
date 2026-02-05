# Asymmetric Drift Scenario (E34_asym_drift)

Defined in `tests/peer_bench.cpp`.

- Drift A: 0 ppm
- Drift B: +100 ppm
- Drift step at 20s: +100 ppm on B only
- Purpose: stress methods that assume anti-symmetric drift across directions.

## E37_asym_drift_step
- Drift A: +50 ppm
- Drift B: 0 ppm
- Drift step at 30s: +100 ppm on A only
- Purpose: asymmetric drift + step on a single side

## E38_asym_drift_netmix
- Drift A: +75 ppm
- Drift B: -25 ppm
- AB-only queueing wave (40ms amp, 4s period) + spikes (+120ms at 0.2%)
- Purpose: asymmetric drift combined with unilateral network pulses
