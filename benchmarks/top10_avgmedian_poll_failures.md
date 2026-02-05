# Top 10 avg-median poll_time_err_p95: worst scenarios per method

Source: benchmarks/run_core_20260203b.csv
Metric: poll_time_err_p95 median per scenario, valid samples only.
Worst = largest ratio vs per-scenario best.

## M4_TimeSyncSkewCorrected:probe_skew_0p2hz
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9827.8 | 391.8 | 9436.0 | 25.09 |
| E7_drift_200ppm | 12721.8 | 737.2 | 11984.5 | 17.26 |
| E7_drift_400ppm | 25666.2 | 1508.0 | 24158.2 | 17.02 |
| E7_drift_300ppm | 19862.8 | 1203.0 | 18659.8 | 16.51 |
| E7_drift_100ppm | 7247.2 | 540.5 | 6706.8 | 13.41 |
| E94_drift_sym_pathchange | 4813.2 | 429.2 | 4384.0 | 11.21 |

## M4_TimeSync
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncSloped:w2s
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncHybrid:t50_s5
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncPerDirSkew:q0.1ppm_4B
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncPerDirSkew:q1ppm_2B
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncPerDirSkew:gate50
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncHybrid:t25_s5
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncPerDirSkew:peer_only
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

## M4_TimeSyncPerDirSkew
| Scenario | This | Best | Gap | Ratio |
|---|---:|---:|---:|---:|
| E114_skew_sign_flip | 9842.5 | 391.8 | 9450.8 | 25.12 |
| E7_drift_200ppm | 12783.0 | 737.2 | 12045.8 | 17.34 |
| E7_drift_400ppm | 25782.8 | 1508.0 | 24274.8 | 17.10 |
| E7_drift_300ppm | 20150.8 | 1203.0 | 18947.8 | 16.75 |
| E7_drift_100ppm | 7222.0 | 540.5 | 6681.5 | 13.36 |
| E94_drift_sym_pathchange | 4820.2 | 429.2 | 4391.0 | 11.23 |

