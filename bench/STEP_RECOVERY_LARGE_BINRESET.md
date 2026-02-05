# Step Scenario Compare (E14–E21)

Source: benchmarks/run_step_recovery_large_20260203_132838/peer_bench.csv

## All poll rates

| method | estimator | discipline | recovery_med_s | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 134218000 | 0 | 134218000 | 0 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 134218000 | 138851000 | 134218000 | 128232000 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 127109000 | 125111000 | 127109000 | 100500000 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 127109000 | 125111000 | 127109000 | 100500000 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 134218000 | 110611000 | 134218000 | 89023800 |

### Step Recovery Times (seconds, median across seeds)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (3 consecutive polls under threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (5 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (5 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (10 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (10 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (20 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (20 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (50 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (50 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (100 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (100 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (500 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (500 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
## Poll rate 1 Hz

| method | estimator | discipline | recovery_med_s | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 134218000 | 0 | 134218000 | 0 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 134218000 | 138851000 | 134218000 | 128232000 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 127109000 | 125111000 | 127109000 | 100500000 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 127109000 | 125111000 | 127109000 | 100500000 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 134218000 | 110611000 | 134218000 | 89023800 |

### Step Recovery Times (seconds, median across seeds)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (3 consecutive polls under threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (5 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (5 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (10 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (10 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (20 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (20 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (50 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (50 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (100 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (100 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (500 ms threshold)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |

### Step Recovery Times (500 ms threshold, 3 consecutive polls)

| method | estimator | discipline | E67_clock_step_120s_a_fwd | E68_clock_step_120s_a_back | E69_clock_step_120s_b_fwd | E70_clock_step_120s_b_back |
| --- | --- | --- | --- | --- | --- | --- |
| M4_TimeSync | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncShadow:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncStepGuard:binreset | min | none | 0.00 | 0.00 | 0.00 | 0.00 |
| M4_TimeSyncDDAC:step | min | none | 0.00 | 0.00 | 0.00 | 0.00 |