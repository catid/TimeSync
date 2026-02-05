#include <TimeSync/TimeSync.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

using std::string;

//------------------------------------------------------------------------------
// PCG PRNG (minimal)

class PCGRandom
{
public:
    void Seed(uint64_t y, uint64_t x = 0)
    {
        State = 0;
        Inc = (y << 1u) | 1u;
        Next();
        State += x;
        Next();
    }

    uint32_t Next()
    {
        const uint64_t oldstate = State;
        State = oldstate * UINT64_C(6364136223846793005) + Inc;
        const uint32_t xorshifted = (uint32_t)(((oldstate >> 18) ^ oldstate) >> 27);
        const uint32_t rot = oldstate >> 59;
        return (xorshifted >> rot) | (xorshifted << ((uint32_t)(-(int32_t)rot) & 31));
    }

    double NextDouble01()
    {
        return (double)Next() / ((double)UINT32_MAX + 1.0);
    }

    uint64_t State = 0, Inc = 0;
};

static double RandNormal(PCGRandom& rng)
{
    double u1 = rng.NextDouble01();
    double u2 = rng.NextDouble01();
    if (u1 < 1e-12) {
        u1 = 1e-12;
    } else if (u1 >= 1.0) {
        u1 = 1.0 - 1e-12;
    }
    const double mag = std::sqrt(-2.0 * std::log(u1));
    const double z0 = mag * std::cos(2.0 * 3.14159265358979323846 * u2);
    return z0;
}

static uint32_t RandRange(PCGRandom& rng, uint32_t minv, uint32_t maxv)
{
    if (maxv <= minv) {
        return minv;
    }
    return minv + (rng.Next() % (maxv - minv + 1));
}

static double RandRangeDouble(PCGRandom& rng, double minv, double maxv)
{
    if (maxv <= minv) {
        return minv;
    }
    const double t = rng.NextDouble01();
    return minv + (maxv - minv) * t;
}

//------------------------------------------------------------------------------
// Stats

struct P2Quantile
{
    explicit P2Quantile(double quantile = 0.5)
        : p(quantile)
    {
    }

    void Add(double x)
    {
        if (count < 5) {
            init.push_back(x);
            ++count;
            if (count == 5) {
                std::sort(init.begin(), init.end());
                for (int i = 0; i < 5; ++i) {
                    q[i] = init[i];
                    n[i] = i + 1;
                }
                np[0] = 1.0;
                np[1] = 1.0 + 2.0 * p;
                np[2] = 1.0 + 4.0 * p;
                np[3] = 3.0 + 2.0 * p;
                np[4] = 5.0;
                dn[0] = 0.0;
                dn[1] = p / 2.0;
                dn[2] = p;
                dn[3] = (1.0 + p) / 2.0;
                dn[4] = 1.0;
            }
            return;
        }

        int k = 0;
        if (x < q[0]) {
            q[0] = x;
            k = 0;
        } else if (x < q[1]) {
            k = 0;
        } else if (x < q[2]) {
            k = 1;
        } else if (x < q[3]) {
            k = 2;
        } else if (x < q[4]) {
            k = 3;
        } else {
            q[4] = x;
            k = 3;
        }

        for (int i = k + 1; i < 5; ++i) {
            n[i] += 1;
        }
        for (int i = 0; i < 5; ++i) {
            np[i] += dn[i];
        }

        for (int i = 1; i < 4; ++i) {
            const double d = np[i] - n[i];
            if ((d >= 1.0 && (n[i + 1] - n[i]) > 1) ||
                (d <= -1.0 && (n[i - 1] - n[i]) < -1)) {
                const int ds = (d >= 0.0) ? 1 : -1;
                const double n_im1 = (double)n[i - 1];
                const double n_i = (double)n[i];
                const double n_ip1 = (double)n[i + 1];
                const double qn = q[i] + (double)ds / (n_ip1 - n_im1) *
                    ((n_i - n_im1 + ds) * (q[i + 1] - q[i]) / (n_ip1 - n_i) +
                     (n_ip1 - n_i - ds) * (q[i] - q[i - 1]) / (n_i - n_im1));
                if (q[i - 1] < qn && qn < q[i + 1]) {
                    q[i] = qn;
                } else if (n[i + ds] != n[i]) {
                    q[i] += (double)ds * (q[i + ds] - q[i]) / (double)(n[i + ds] - n[i]);
                }
                n[i] += ds;
            }
        }
    }

    bool Ready() const { return count >= 5; }

    double Value() const
    {
        if (count == 0) {
            return 0.0;
        }
        if (count < 5) {
            std::vector<double> tmp = init;
            std::sort(tmp.begin(), tmp.end());
            const size_t idx = (size_t)std::ceil(p * (double)(tmp.size() - 1));
            return tmp[idx];
        }
        return q[2];
    }

    double p = 0.5;
    int count = 0;
    double q[5] = {0, 0, 0, 0, 0};
    int n[5] = {0, 0, 0, 0, 0};
    double np[5] = {0, 0, 0, 0, 0};
    double dn[5] = {0, 0, 0, 0, 0};
    std::vector<double> init;
};

struct SampleStats
{
    void Add(uint32_t v)
    {
        ++count;
        sum += v;
        if (v < min) {
            min = v;
        }
        if (v > max) {
            max = v;
        }

        if (exact_samples.size() < kExactMax) {
            exact_samples.push_back(v);
            sorted = false;
        }

        p95.Add((double)v);
        p99.Add((double)v);
    }

    size_t Count() const
    {
        return count;
    }

    double Mean() const
    {
        if (count == 0) {
            return 0.0;
        }
        return (double)sum / (double)count;
    }

    double Percentile(double p) const
    {
        if (count == 0) {
            return 0.0;
        }
        if (p <= 0.0) {
            return Min();
        }
        if (p >= 1.0) {
            return Max();
        }
        if (count <= kExactMax) {
            EnsureSorted();
            const size_t n = exact_samples.size();
            const size_t idx = (size_t)std::ceil(p * (double)(n - 1));
            return exact_samples[idx];
        }
        if (std::fabs(p - 0.95) < 1e-6) {
            return p95.Value();
        }
        if (std::fabs(p - 0.99) < 1e-6) {
            return p99.Value();
        }
        return Min() + (Max() - Min()) * p;
    }

    uint32_t Min() const { return (count == 0) ? 0 : min; }
    uint32_t Max() const { return (count == 0) ? 0 : max; }

    void Reserve(size_t)
    {
        // bounded storage only
    }

private:
    void EnsureSorted() const
    {
        if (sorted) {
            return;
        }
        std::sort(exact_samples.begin(), exact_samples.end());
        sorted = true;
    }

    static const size_t kExactMax = 2048;
    size_t count = 0;
    uint64_t sum = 0;
    uint32_t min = UINT32_MAX;
    uint32_t max = 0;
    mutable bool sorted = false;
    mutable std::vector<uint32_t> exact_samples;
    P2Quantile p95 = P2Quantile(0.95);
    P2Quantile p99 = P2Quantile(0.99);
};

//------------------------------------------------------------------------------
// Models

enum class JitterMode
{
    Uniform,
    NormalAbs,
    Gaussian,
    LogNormal,
    Pareto,
    Bimodal
};

enum class NoiseMode
{
    Uniform,
    LogNormal
};

struct DelayModel
{
    uint32_t base_delay_us = 50000;
    uint32_t jitter_us = 1000;
    JitterMode jitter_mode = JitterMode::Uniform;
    double jitter_clip_sigma = 0.0;
    double lognormal_sigma = 0.5;
    double pareto_alpha = 2.0;
    uint32_t pareto_scale_us = 1000;
    double spike_prob = 0.0;
    uint32_t spike_delay_us = 0;
    double bimodal_prob = 0.0;
    uint32_t bimodal_delay_us = 0;
    double bimodal_high_prob = 0.2;
    uint32_t queue_amp_us = 0;
    uint64_t queue_period_us = 0;
    int32_t ramp_us_per_s = 0;
    uint64_t step_at_us = 0;
    int32_t step_delta_us = 0;
    uint64_t step2_at_us = 0;
    int32_t step2_delta_us = 0;
    uint64_t saw_period_us = 0;
    uint32_t saw_amp_us = 0;
    uint64_t rw_step_interval_us = 0;
    int32_t rw_step_us = 0;
    uint32_t rw_max_us = 0;
    uint64_t rw_last_update_us = 0;
    int32_t rw_value_us = 0;

    uint32_t SampleDelay(uint64_t now_us, PCGRandom& rng)
    {
        int64_t delay = base_delay_us;

        if (ramp_us_per_s != 0) {
            delay += (int64_t)((double)ramp_us_per_s * (double)now_us / 1000000.0);
        }

        if (step_at_us > 0 && now_us >= step_at_us) {
            delay += step_delta_us;
        }
        if (step2_at_us > 0 && now_us >= step2_at_us) {
            delay += step2_delta_us;
        }

        if (queue_amp_us > 0 && queue_period_us > 0) {
            const double phase = 2.0 * 3.14159265358979323846 *
                                 (double)(now_us % queue_period_us) / (double)queue_period_us;
            const double q = 0.5 * (1.0 + std::sin(phase));
            delay += (int64_t)(q * (double)queue_amp_us);
        }

        if (saw_period_us > 0 && saw_amp_us > 0) {
            const double phase = (double)(now_us % saw_period_us) / (double)saw_period_us;
            delay += (int64_t)(phase * (double)saw_amp_us);
        }

        if (rw_step_interval_us > 0 && rw_step_us != 0) {
            if (rw_last_update_us == 0) {
                rw_last_update_us = now_us;
            }
            while (rw_last_update_us + rw_step_interval_us <= now_us) {
                rw_last_update_us += rw_step_interval_us;
                const double step = RandNormal(rng) * (double)rw_step_us;
                rw_value_us += (int32_t)step;
                if (rw_max_us > 0) {
                    if (rw_value_us < 0) rw_value_us = 0;
                    if ((uint32_t)rw_value_us > rw_max_us) rw_value_us = (int32_t)rw_max_us;
                }
            }
            delay += rw_value_us;
        }

        if (jitter_us > 0) {
            if (jitter_mode == JitterMode::Uniform) {
                delay += RandRange(rng, 0, jitter_us);
            }
            else if (jitter_mode == JitterMode::NormalAbs) {
                const double j = std::fabs(RandNormal(rng) * (double)jitter_us);
                delay += (int64_t)j;
            }
            else if (jitter_mode == JitterMode::Gaussian) {
                double j = RandNormal(rng) * (double)jitter_us;
                if (jitter_clip_sigma > 0.0) {
                    const double limit = jitter_clip_sigma * (double)jitter_us;
                    if (j > limit) j = limit;
                    else if (j < -limit) j = -limit;
                }
                delay += (int64_t)j;
            }
            else if (jitter_mode == JitterMode::LogNormal) {
                const double sigma = lognormal_sigma;
                const double mu = std::log((double)jitter_us + 1.0);
                const double j = std::exp(mu + sigma * RandNormal(rng)) - 1.0;
                delay += (int64_t)j;
            }
            else if (jitter_mode == JitterMode::Pareto) {
                const double u = 1.0 - rng.NextDouble01();
                const double alpha = pareto_alpha;
                const double xm = (double)pareto_scale_us;
                const double j = xm / std::pow(u, 1.0 / alpha);
                delay += (int64_t)j;
            }
            else if (jitter_mode == JitterMode::Bimodal) {
                const double mode = (rng.NextDouble01() < bimodal_high_prob) ? (double)bimodal_delay_us : 0.0;
                delay += (int64_t)mode;
            }
        }

        if (bimodal_prob > 0.0 && rng.NextDouble01() < bimodal_prob) {
            delay += bimodal_delay_us;
        }

        if (spike_prob > 0.0 && rng.NextDouble01() < spike_prob) {
            delay += spike_delay_us;
        }

        if (delay < 0) {
            delay = 0;
        }

        return (uint32_t)delay;
    }
};

struct LossModel
{
    double loss_rate = 0.0;
    double burst_start_prob = 0.0;
    uint32_t burst_len_min = 0;
    uint32_t burst_len_max = 0;
    uint32_t burst_remaining = 0;
    uint32_t periodic_n = 0;
    uint32_t packet_counter = 0;
    uint32_t delay_drop_threshold_us = 0;
    double delay_drop_prob = 1.0;

    bool ShouldDrop(PCGRandom& rng, uint32_t delay_us)
    {
        ++packet_counter;

        if (periodic_n > 0 && (packet_counter % periodic_n) == 0) {
            return true;
        }

        if (delay_drop_threshold_us > 0 && delay_us > delay_drop_threshold_us) {
            if (delay_drop_prob >= 1.0 || rng.NextDouble01() < delay_drop_prob) {
                return true;
            }
        }

        if (burst_remaining > 0) {
            --burst_remaining;
            return true;
        }

        if (burst_start_prob > 0.0 && rng.NextDouble01() < burst_start_prob) {
            uint32_t len = 0;
            if (burst_len_max >= burst_len_min && burst_len_max > 0) {
                len = RandRange(rng, burst_len_min, burst_len_max);
            }
            if (len > 0) {
                burst_remaining = len - 1;
                return true;
            }
        }

        if (loss_rate > 0.0 && rng.NextDouble01() < loss_rate) {
            return true;
        }

        return false;
    }
};

//------------------------------------------------------------------------------
// Simulation

struct ExperimentConfig
{
    string name;
    uint64_t duration_us = 30 * 1000 * 1000ULL;
    double send_rate_hz = 60.0;
    double poll_rate_hz = 10.0;
    uint64_t sync_interval_us = 2 * 1000 * 1000ULL;
    uint64_t sync_interval_ab_us = 0;
    uint64_t sync_interval_ba_us = 0;
    uint64_t metrics_warmup_us = 1000 * 1000ULL;

    DelayModel delay_ab;
    DelayModel delay_ba;
    LossModel loss_ab;
    LossModel loss_ba;
    LossModel loss_sync_ab;
    LossModel loss_sync_ba;
    bool use_sync_loss = false;

    double reorder_prob_ab = 0.0;
    double reorder_prob_ba = 0.0;
    uint32_t reorder_advance_us = 0;
    uint32_t reorder_delay_us = 0;
    double duplicate_prob_ab = 0.0;
    double duplicate_prob_ba = 0.0;
    uint32_t duplicate_delay_us = 1000;

    bool inject_late_ab = false;
    bool inject_late_ba = false;
    uint64_t inject_late_at_us = 0;
    uint32_t inject_late_delay_us = 0;

    double drift_ppm_a = 0.0;
    double drift_ppm_b = 0.0;
    double drift_sine_amp_ppm = 0.0;
    uint64_t drift_sine_period_us = 0;
    double drift_rw_step_ppm = 0.0;
    uint64_t drift_rw_step_interval_us = 1000000;
    bool drift_step_enabled = false;
    uint64_t drift_step_time_us = 0;
    double drift_step_delta_ppm = 0.0;
    double drift_step_delta_ppm_a = 0.0;
    double drift_step_delta_ppm_b = 0.0;

    int64_t offset_a_us = 0;
    int64_t offset_b_us = 5 * 1000 * 1000LL;
    uint32_t quantize_us = 0;
    uint32_t recv_noise_us = 0;
    NoiseMode recv_noise_mode = NoiseMode::Uniform;
    double recv_noise_sigma = 0.5;
    bool clock_step_enabled = false;
    uint64_t clock_step_time_us = 0;
    int64_t clock_step_a_us = 0;
    int64_t clock_step_b_us = 0;
    uint64_t drift_window_us = 0;
    uint64_t blackout_start_us = 0;
    uint64_t blackout_end_us = 0;
};

struct Packet
{
    bool from_a = true;
    bool is_sync = false;
    bool app_ts_valid = false;
    Counter24 ts24 = 0;
    Counter23 app_ts23 = 0;
    Counter24 min_delta = 0;
    uint64_t send_true_us = 0;
    uint64_t true_remote_local_at_send = 0;
    uint32_t true_delay_us = 0;
};

struct ArrivalEvent
{
    uint64_t deliver_true_us = 0;
    Packet packet;
};

struct ArrivalCompare
{
    bool operator()(const ArrivalEvent& a, const ArrivalEvent& b) const
    {
        return a.deliver_true_us > b.deliver_true_us;
    }
};

struct DirectionMetrics
{
    uint64_t sent = 0;
    uint64_t received = 0;
    uint64_t lost = 0;
    uint64_t sync_sent = 0;

    SampleStats time_error_us;
    SampleStats poll_time_error_us;
    SampleStats owd_error_us;

    uint32_t min_true_owd_us = UINT32_MAX;
};

struct ExperimentMetrics
{
    bool sync_a = false;
    bool sync_b = false;
    bool metrics_start_set = false;

    uint64_t sync_time_a_us = 0;
    uint64_t sync_time_b_us = 0;
    uint64_t metrics_start_us = 0;

    DirectionMetrics ab;
    DirectionMetrics ba;

    uint32_t min_owd_est_a_us = 0;
    uint32_t min_owd_est_b_us = 0;
};

struct ClockModel
{
    double drift_ppm_base = 0.0;
    double drift_sine_amp_ppm = 0.0;
    uint64_t drift_sine_period_us = 0;
    double drift_rw_step_ppm = 0.0;
    uint64_t drift_rw_step_interval_us = 1000000;
    double drift_rw_ppm = 0.0;
    uint64_t drift_rw_last_us = 0;
    bool drift_step_enabled = false;
    uint64_t drift_step_time_us = 0;
    double drift_step_delta_ppm = 0.0;

    int64_t offset_us = 0;
    uint32_t quantize_us = 0;
    uint32_t recv_noise_us = 0;
    NoiseMode recv_noise_mode = NoiseMode::Uniform;
    double recv_noise_sigma = 0.5;
    bool clock_step_enabled = false;
    uint64_t clock_step_time_us = 0;
    int64_t clock_step_us = 0;
};

static void UpdateDriftWander(ClockModel& model, uint64_t now_us, PCGRandom& rng)
{
    if (model.drift_rw_step_ppm == 0.0 || model.drift_rw_step_interval_us == 0) {
        return;
    }
    if (model.drift_rw_last_us == 0) {
        model.drift_rw_last_us = now_us;
    }
    while (model.drift_rw_last_us + model.drift_rw_step_interval_us <= now_us) {
        model.drift_rw_last_us += model.drift_rw_step_interval_us;
        const double step = RandNormal(rng) * model.drift_rw_step_ppm;
        model.drift_rw_ppm += step;
    }
}

static uint64_t ComputeLocalTimeUsec(uint64_t true_us, ClockModel& model, PCGRandom& rng, bool add_recv_noise)
{
    UpdateDriftWander(model, true_us, rng);

    double drift_ppm = model.drift_ppm_base + model.drift_rw_ppm;
    if (model.drift_sine_amp_ppm != 0.0 && model.drift_sine_period_us > 0) {
        const double phase = 2.0 * 3.14159265358979323846 *
                             (double)(true_us % model.drift_sine_period_us) /
                             (double)model.drift_sine_period_us;
        drift_ppm += model.drift_sine_amp_ppm * std::sin(phase);
    }
    if (model.drift_step_enabled && true_us >= model.drift_step_time_us) {
        drift_ppm += model.drift_step_delta_ppm;
    }

    const double scale = 1.0 + drift_ppm * 1e-6;
    double local = (double)model.offset_us + (double)true_us * scale;

    if (model.clock_step_enabled && true_us >= model.clock_step_time_us) {
        local += (double)model.clock_step_us;
    }

    if (model.quantize_us > 0) {
        const double q = (double)model.quantize_us;
        local = std::floor(local / q) * q;
    }

    if (add_recv_noise && model.recv_noise_us > 0) {
        if (model.recv_noise_mode == NoiseMode::LogNormal) {
            const double sigma = model.recv_noise_sigma;
            const double mu = std::log((double)model.recv_noise_us + 1.0);
            const double j = std::exp(mu + sigma * RandNormal(rng)) - 1.0;
            local += j;
        }
        else {
            local += RandRange(rng, 0, model.recv_noise_us);
        }
    }

    if (local <= 0.0) {
        return 0;
    }
    return (uint64_t)std::llround(local);
}

static void MaybeSetSyncTime(TimeSynchronizer& sync, uint64_t now_us, bool& flag, uint64_t& out)
{
    if (!flag && sync.IsSynchronized()) {
        flag = true;
        out = now_us;
    }
}

static uint64_t ClampMetricsWarmup(uint64_t warmup_us, uint64_t duration_us)
{
    if (duration_us == 0) {
        return warmup_us;
    }
    if (warmup_us <= (duration_us / 2)) {
        return warmup_us;
    }
    return duration_us / 10;
}

static ExperimentMetrics RunExperiment(
    const ExperimentConfig& cfg,
    uint64_t seed,
    std::atomic<uint32_t>* progress_permille = nullptr)
{
    PCGRandom rng;
    rng.Seed(seed, 0x12345678ULL);

    TimeSynchronizer sync_a;
    TimeSynchronizer sync_b;

    if (cfg.drift_window_us > 0) {
        sync_a.SetDriftWindowUsec(cfg.drift_window_us);
        sync_b.SetDriftWindowUsec(cfg.drift_window_us);
    }

    DelayModel delay_ab = cfg.delay_ab;
    DelayModel delay_ba = cfg.delay_ba;
    LossModel loss_ab = cfg.loss_ab;
    LossModel loss_ba = cfg.loss_ba;
    LossModel loss_sync_ab = cfg.use_sync_loss ? cfg.loss_sync_ab : cfg.loss_ab;
    LossModel loss_sync_ba = cfg.use_sync_loss ? cfg.loss_sync_ba : cfg.loss_ba;

    ClockModel clock_a;
    clock_a.drift_ppm_base = cfg.drift_ppm_a;
    clock_a.drift_sine_amp_ppm = cfg.drift_sine_amp_ppm;
    clock_a.drift_sine_period_us = cfg.drift_sine_period_us;
    clock_a.drift_rw_step_ppm = cfg.drift_rw_step_ppm;
    clock_a.drift_rw_step_interval_us = cfg.drift_rw_step_interval_us;
    clock_a.drift_step_enabled = cfg.drift_step_enabled;
    clock_a.drift_step_time_us = cfg.drift_step_time_us;
    clock_a.drift_step_delta_ppm = (cfg.drift_step_delta_ppm_a != 0.0)
        ? cfg.drift_step_delta_ppm_a
        : cfg.drift_step_delta_ppm;
    clock_a.offset_us = cfg.offset_a_us;
    clock_a.quantize_us = cfg.quantize_us;
    clock_a.recv_noise_us = cfg.recv_noise_us;
    clock_a.recv_noise_mode = cfg.recv_noise_mode;
    clock_a.recv_noise_sigma = cfg.recv_noise_sigma;
    clock_a.clock_step_enabled = cfg.clock_step_enabled;
    clock_a.clock_step_time_us = cfg.clock_step_time_us;
    clock_a.clock_step_us = cfg.clock_step_a_us;

    ClockModel clock_b = clock_a;
    clock_b.drift_ppm_base = cfg.drift_ppm_b;
    clock_b.drift_step_delta_ppm = (cfg.drift_step_delta_ppm_b != 0.0)
        ? cfg.drift_step_delta_ppm_b
        : clock_a.drift_step_delta_ppm;
    clock_b.offset_us = cfg.offset_b_us;
    clock_b.clock_step_us = cfg.clock_step_b_us;

    const uint64_t data_interval_us = (cfg.send_rate_hz > 0.0)
        ? (uint64_t)std::llround(1000000.0 / cfg.send_rate_hz)
        : 0;
    const uint64_t poll_interval_us = (cfg.poll_rate_hz > 0.0)
        ? (uint64_t)std::llround(1000000.0 / cfg.poll_rate_hz)
        : 0;
    const uint64_t sync_interval_ab = (cfg.sync_interval_ab_us > 0)
        ? cfg.sync_interval_ab_us
        : cfg.sync_interval_us;
    const uint64_t sync_interval_ba = (cfg.sync_interval_ba_us > 0)
        ? cfg.sync_interval_ba_us
        : cfg.sync_interval_us;

    uint64_t next_data_ab = (data_interval_us > 0) ? 0 : UINT64_MAX;
    uint64_t next_data_ba = (data_interval_us > 0) ? 0 : UINT64_MAX;
    uint64_t next_poll_ab = (poll_interval_us > 0) ? 0 : UINT64_MAX;
    uint64_t next_poll_ba = (poll_interval_us > 0) ? 0 : UINT64_MAX;
    uint64_t next_sync_ab = (sync_interval_ab > 0) ? sync_interval_ab : UINT64_MAX;
    uint64_t next_sync_ba = (sync_interval_ba > 0) ? sync_interval_ba : UINT64_MAX;

    uint64_t now_us = 0;
    bool late_ab_injected = false;
    bool late_ba_injected = false;

    std::priority_queue<ArrivalEvent, std::vector<ArrivalEvent>, ArrivalCompare> arrivals;

    ExperimentMetrics metrics;
    const double expected_packets = (double)cfg.duration_us * cfg.send_rate_hz / 1000000.0;
    const size_t reserve_count = (size_t)std::ceil(expected_packets);
    metrics.ab.time_error_us.Reserve(reserve_count);
    metrics.ab.owd_error_us.Reserve(reserve_count);
    metrics.ba.time_error_us.Reserve(reserve_count);
    metrics.ba.owd_error_us.Reserve(reserve_count);

    uint64_t next_progress_update_us = 0;
    const uint64_t progress_step_us = (cfg.duration_us > 0)
        ? std::max<uint64_t>(cfg.duration_us / 100, 1000)
        : 1000;
    if (progress_permille) {
        progress_permille->store(0, std::memory_order_relaxed);
    }

    while (now_us <= cfg.duration_us) {
        uint64_t next_arrival = arrivals.empty() ? UINT64_MAX : arrivals.top().deliver_true_us;
        uint64_t next_time = next_arrival;
        if (next_data_ab < next_time) next_time = next_data_ab;
        if (next_data_ba < next_time) next_time = next_data_ba;
        if (next_poll_ab < next_time) next_time = next_poll_ab;
        if (next_poll_ba < next_time) next_time = next_poll_ba;
        if (next_sync_ab < next_time) next_time = next_sync_ab;
        if (next_sync_ba < next_time) next_time = next_sync_ba;

        if (next_time == UINT64_MAX) {
            break;
        }

        now_us = next_time;
        if (progress_permille && now_us >= next_progress_update_us) {
            uint32_t permille = 1000;
            if (cfg.duration_us > 0) {
                permille = (uint32_t)std::min<uint64_t>(1000, (now_us * 1000) / cfg.duration_us);
            }
            progress_permille->store(permille, std::memory_order_relaxed);
            next_progress_update_us = now_us + progress_step_us;
        }
        const bool in_blackout = (cfg.blackout_end_us > cfg.blackout_start_us) &&
            (now_us >= cfg.blackout_start_us && now_us < cfg.blackout_end_us);

        // Deliver packets scheduled for now
        while (!arrivals.empty() && arrivals.top().deliver_true_us == now_us) {
            ArrivalEvent ev = arrivals.top();
            arrivals.pop();

            const bool to_b = ev.packet.from_a;
            TimeSynchronizer& recv_sync = to_b ? sync_b : sync_a;
            ClockModel& recv_clock = to_b ? clock_b : clock_a;
            DirectionMetrics& dir = to_b ? metrics.ab : metrics.ba;

            const uint64_t local_recv = ComputeLocalTimeUsec(now_us, recv_clock, rng, true);

            const unsigned owd_est = recv_sync.OnAuthenticatedDatagramTimestamp(
                ev.packet.ts24, local_recv);

            if (ev.packet.is_sync) {
                recv_sync.OnPeerMinDeltaTS24(ev.packet.min_delta);
            }

            dir.received++;

            if (ev.packet.true_delay_us < dir.min_true_owd_us) {
                dir.min_true_owd_us = ev.packet.true_delay_us;
            }

            if (to_b) {
                MaybeSetSyncTime(sync_b, now_us, metrics.sync_b, metrics.sync_time_b_us);
            }
            else {
                MaybeSetSyncTime(sync_a, now_us, metrics.sync_a, metrics.sync_time_a_us);
            }

            if (!metrics.metrics_start_set && metrics.sync_a && metrics.sync_b) {
                const uint64_t sync_max = (metrics.sync_time_a_us > metrics.sync_time_b_us)
                    ? metrics.sync_time_a_us
                    : metrics.sync_time_b_us;
                const uint64_t warmup_us = ClampMetricsWarmup(cfg.metrics_warmup_us, cfg.duration_us);
                metrics.metrics_start_us = sync_max + warmup_us;
                metrics.metrics_start_set = true;
            }

            const bool metrics_ready = metrics.metrics_start_set && now_us >= metrics.metrics_start_us;
            if (metrics_ready && recv_sync.IsSynchronized()) {
                if (owd_est > 0) {
                    const uint32_t err = (owd_est > ev.packet.true_delay_us)
                        ? (owd_est - ev.packet.true_delay_us)
                        : (ev.packet.true_delay_us - owd_est);
                    dir.owd_error_us.Add(err);
                }

                if (ev.packet.app_ts_valid) {
                    const uint64_t estimated_local = recv_sync.FromLocalTime23(
                        local_recv,
                        ev.packet.app_ts23);
                    const uint64_t true_local_at_send = ev.packet.true_remote_local_at_send;
                    const uint64_t err = (estimated_local > true_local_at_send)
                        ? (estimated_local - true_local_at_send)
                        : (true_local_at_send - estimated_local);
                    dir.time_error_us.Add((uint32_t)err);
                }
            }
        }

        if (next_poll_ab == now_us) {
            const uint64_t local_now = ComputeLocalTimeUsec(now_us, clock_a, rng, false);
            uint64_t remote_est = 0;
            if (sync_a.GetRemoteTimeUsec(local_now, remote_est)) {
                if (metrics.metrics_start_set && now_us >= metrics.metrics_start_us) {
                    const uint64_t true_remote = ComputeLocalTimeUsec(now_us, clock_b, rng, false);
                    const double err = (double)remote_est - (double)true_remote;
                    metrics.ab.poll_time_error_us.Add((uint32_t)std::llround(std::fabs(err)));
                }
            }
            next_poll_ab = (poll_interval_us > 0) ? (now_us + poll_interval_us) : UINT64_MAX;
        }

        if (next_poll_ba == now_us) {
            const uint64_t local_now = ComputeLocalTimeUsec(now_us, clock_b, rng, false);
            uint64_t remote_est = 0;
            if (sync_b.GetRemoteTimeUsec(local_now, remote_est)) {
                if (metrics.metrics_start_set && now_us >= metrics.metrics_start_us) {
                    const uint64_t true_remote = ComputeLocalTimeUsec(now_us, clock_a, rng, false);
                    const double err = (double)remote_est - (double)true_remote;
                    metrics.ba.poll_time_error_us.Add((uint32_t)std::llround(std::fabs(err)));
                }
            }
            next_poll_ba = (poll_interval_us > 0) ? (now_us + poll_interval_us) : UINT64_MAX;
        }

        if (cfg.inject_late_ab && !late_ab_injected && now_us >= cfg.inject_late_at_us) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, clock_a, rng, false);
            Packet pkt;
            pkt.from_a = true;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_a.LocalTimeToDatagramTS24(local_send);
            pkt.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, clock_b, rng, false);
            pkt.true_delay_us = cfg.inject_late_delay_us;
            ArrivalEvent ev;
            ev.deliver_true_us = now_us + pkt.true_delay_us;
            ev.packet = pkt;
            arrivals.push(ev);
            late_ab_injected = true;
        }

        if (cfg.inject_late_ba && !late_ba_injected && now_us >= cfg.inject_late_at_us) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, clock_b, rng, false);
            Packet pkt;
            pkt.from_a = false;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_b.LocalTimeToDatagramTS24(local_send);
            pkt.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, clock_a, rng, false);
            pkt.true_delay_us = cfg.inject_late_delay_us;
            ArrivalEvent ev;
            ev.deliver_true_us = now_us + pkt.true_delay_us;
            ev.packet = pkt;
            arrivals.push(ev);
            late_ba_injected = true;
        }

        // Send data A->B
        if (next_data_ab == now_us) {
            if (in_blackout) {
                next_data_ab = (data_interval_us > 0) ? (now_us + data_interval_us) : UINT64_MAX;
            }
            else {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, clock_a, rng, false);

            Packet pkt;
            pkt.from_a = true;
            pkt.is_sync = false;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_a.LocalTimeToDatagramTS24(local_send);
            pkt.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, clock_b, rng, false);

            if (sync_a.IsSynchronized()) {
                pkt.app_ts23 = sync_a.ToRemoteTime23(local_send);
                pkt.app_ts_valid = pkt.app_ts23 != 0;
            }

            metrics.ab.sent++;

            pkt.true_delay_us = delay_ab.SampleDelay(now_us, rng);
            if (cfg.reorder_prob_ab > 0.0 && rng.NextDouble01() < cfg.reorder_prob_ab) {
                if (cfg.reorder_delay_us > 0) {
                    pkt.true_delay_us += cfg.reorder_delay_us;
                }
                else {
                    const uint32_t advance = (cfg.reorder_advance_us > 0) ? cfg.reorder_advance_us : (pkt.true_delay_us / 2);
                    pkt.true_delay_us = (pkt.true_delay_us > advance) ? (pkt.true_delay_us - advance) : 0;
                }
            }

            const bool drop = loss_ab.ShouldDrop(rng, pkt.true_delay_us);
            if (drop) {
                metrics.ab.lost++;
            }
            else {
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);

                if (cfg.duplicate_prob_ab > 0.0 && rng.NextDouble01() < cfg.duplicate_prob_ab) {
                    ArrivalEvent dup = ev;
                    dup.deliver_true_us = ev.deliver_true_us + cfg.duplicate_delay_us;
                    arrivals.push(dup);
                }
            }

            next_data_ab = (data_interval_us > 0) ? (now_us + data_interval_us) : UINT64_MAX;
            }
        }

        // Send data B->A
        if (next_data_ba == now_us) {
            if (in_blackout) {
                next_data_ba = (data_interval_us > 0) ? (now_us + data_interval_us) : UINT64_MAX;
            }
            else {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, clock_b, rng, false);

            Packet pkt;
            pkt.from_a = false;
            pkt.is_sync = false;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_b.LocalTimeToDatagramTS24(local_send);
            pkt.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, clock_a, rng, false);

            if (sync_b.IsSynchronized()) {
                pkt.app_ts23 = sync_b.ToRemoteTime23(local_send);
                pkt.app_ts_valid = pkt.app_ts23 != 0;
            }

            metrics.ba.sent++;

            pkt.true_delay_us = delay_ba.SampleDelay(now_us, rng);
            if (cfg.reorder_prob_ba > 0.0 && rng.NextDouble01() < cfg.reorder_prob_ba) {
                if (cfg.reorder_delay_us > 0) {
                    pkt.true_delay_us += cfg.reorder_delay_us;
                }
                else {
                    const uint32_t advance = (cfg.reorder_advance_us > 0) ? cfg.reorder_advance_us : (pkt.true_delay_us / 2);
                    pkt.true_delay_us = (pkt.true_delay_us > advance) ? (pkt.true_delay_us - advance) : 0;
                }
            }

            const bool drop = loss_ba.ShouldDrop(rng, pkt.true_delay_us);
            if (drop) {
                metrics.ba.lost++;
            }
            else {
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);

                if (cfg.duplicate_prob_ba > 0.0 && rng.NextDouble01() < cfg.duplicate_prob_ba) {
                    ArrivalEvent dup = ev;
                    dup.deliver_true_us = ev.deliver_true_us + cfg.duplicate_delay_us;
                    arrivals.push(dup);
                }
            }

            next_data_ba = (data_interval_us > 0) ? (now_us + data_interval_us) : UINT64_MAX;
            }
        }

        // Send sync A->B
        if (next_sync_ab == now_us) {
            if (in_blackout) {
                next_sync_ab = (sync_interval_ab > 0) ? (now_us + sync_interval_ab) : UINT64_MAX;
            }
            else {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, clock_a, rng, false);

            Packet pkt;
            pkt.from_a = true;
            pkt.is_sync = true;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_a.LocalTimeToDatagramTS24(local_send);
            pkt.min_delta = sync_a.GetMinDeltaTS24();

            if (sync_a.IsSynchronized()) {
                pkt.app_ts23 = sync_a.ToRemoteTime23(local_send);
                pkt.app_ts_valid = pkt.app_ts23 != 0;
            }

            metrics.ab.sent++;
            metrics.ab.sync_sent++;

            pkt.true_delay_us = delay_ab.SampleDelay(now_us, rng);
            if (cfg.reorder_prob_ab > 0.0 && rng.NextDouble01() < cfg.reorder_prob_ab) {
                if (cfg.reorder_delay_us > 0) {
                    pkt.true_delay_us += cfg.reorder_delay_us;
                }
                else {
                    const uint32_t advance = (cfg.reorder_advance_us > 0) ? cfg.reorder_advance_us : (pkt.true_delay_us / 2);
                    pkt.true_delay_us = (pkt.true_delay_us > advance) ? (pkt.true_delay_us - advance) : 0;
                }
            }

            const bool drop = loss_sync_ab.ShouldDrop(rng, pkt.true_delay_us);
            if (drop) {
                metrics.ab.lost++;
            }
            else {
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);

                if (cfg.duplicate_prob_ab > 0.0 && rng.NextDouble01() < cfg.duplicate_prob_ab) {
                    ArrivalEvent dup = ev;
                    dup.deliver_true_us = ev.deliver_true_us + cfg.duplicate_delay_us;
                    arrivals.push(dup);
                }
            }

            next_sync_ab = (sync_interval_ab > 0) ? (now_us + sync_interval_ab) : UINT64_MAX;
            }
        }

        // Send sync B->A
        if (next_sync_ba == now_us) {
            if (in_blackout) {
                next_sync_ba = (sync_interval_ba > 0) ? (now_us + sync_interval_ba) : UINT64_MAX;
            }
            else {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, clock_b, rng, false);

            Packet pkt;
            pkt.from_a = false;
            pkt.is_sync = true;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_b.LocalTimeToDatagramTS24(local_send);
            pkt.min_delta = sync_b.GetMinDeltaTS24();

            if (sync_b.IsSynchronized()) {
                pkt.app_ts23 = sync_b.ToRemoteTime23(local_send);
                pkt.app_ts_valid = pkt.app_ts23 != 0;
            }

            metrics.ba.sent++;
            metrics.ba.sync_sent++;

            pkt.true_delay_us = delay_ba.SampleDelay(now_us, rng);
            if (cfg.reorder_prob_ba > 0.0 && rng.NextDouble01() < cfg.reorder_prob_ba) {
                if (cfg.reorder_delay_us > 0) {
                    pkt.true_delay_us += cfg.reorder_delay_us;
                }
                else {
                    const uint32_t advance = (cfg.reorder_advance_us > 0) ? cfg.reorder_advance_us : (pkt.true_delay_us / 2);
                    pkt.true_delay_us = (pkt.true_delay_us > advance) ? (pkt.true_delay_us - advance) : 0;
                }
            }

            const bool drop = loss_sync_ba.ShouldDrop(rng, pkt.true_delay_us);
            if (drop) {
                metrics.ba.lost++;
            }
            else {
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);

                if (cfg.duplicate_prob_ba > 0.0 && rng.NextDouble01() < cfg.duplicate_prob_ba) {
                    ArrivalEvent dup = ev;
                    dup.deliver_true_us = ev.deliver_true_us + cfg.duplicate_delay_us;
                    arrivals.push(dup);
                }
            }

            next_sync_ba = (sync_interval_ba > 0) ? (now_us + sync_interval_ba) : UINT64_MAX;
            }
        }
    }

    metrics.min_owd_est_a_us = sync_a.GetMinimumOneWayDelayUsec();
    metrics.min_owd_est_b_us = sync_b.GetMinimumOneWayDelayUsec();

    if (progress_permille) {
        progress_permille->store(1000, std::memory_order_relaxed);
    }

    return metrics;
}

//------------------------------------------------------------------------------
// Experiment definitions

static ExperimentConfig BaseConfig(const string& name)
{
    ExperimentConfig cfg;
    cfg.name = name;
    cfg.delay_ab.base_delay_us = 50000;
    cfg.delay_ba.base_delay_us = 50000;
    cfg.delay_ab.jitter_us = 1000;
    cfg.delay_ba.jitter_us = 1000;
    cfg.delay_ab.jitter_mode = JitterMode::Uniform;
    cfg.delay_ba.jitter_mode = JitterMode::Uniform;
    cfg.offset_a_us = 0;
    cfg.offset_b_us = 5 * 1000 * 1000LL;
    return cfg;
}

static std::vector<ExperimentConfig> BuildExperiments()
{
    std::vector<ExperimentConfig> exps;

    auto add_with_reverse = [&](const ExperimentConfig& cfg, const char* suffix) {
        exps.push_back(cfg);
        if (cfg.drift_ppm_a != 0.0 || cfg.drift_ppm_b != 0.0) {
            ExperimentConfig rev = cfg;
            rev.drift_ppm_a = -cfg.drift_ppm_a;
            rev.drift_ppm_b = -cfg.drift_ppm_b;
            rev.name = cfg.name + suffix;
            exps.push_back(rev);
        }
    };

    // Baseline + jitter sweep (7)
    {
        ExperimentConfig cfg = BaseConfig("baseline_20ms_j0p5");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 500;
        cfg.delay_ba.jitter_us = 500;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("baseline_20ms_j2");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("baseline_20ms_j5");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("baseline_20ms_j10");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 10000;
        cfg.delay_ba.jitter_us = 10000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("baseline_80ms_j2");
        cfg.delay_ab.base_delay_us = 80000;
        cfg.delay_ba.base_delay_us = 80000;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("baseline_80ms_j10");
        cfg.delay_ab.base_delay_us = 80000;
        cfg.delay_ba.base_delay_us = 80000;
        cfg.delay_ab.jitter_us = 10000;
        cfg.delay_ba.jitter_us = 10000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("baseline_150ms_j20");
        cfg.delay_ab.base_delay_us = 150000;
        cfg.delay_ba.base_delay_us = 150000;
        cfg.delay_ab.jitter_us = 20000;
        cfg.delay_ba.jitter_us = 20000;
        exps.push_back(cfg);
    }

    // Asymmetry sweep (6)
    {
        ExperimentConfig cfg = BaseConfig("asym_20_40_j2");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 40000;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("asym_20_80_j2");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 80000;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("asym_50_100_j5");
        cfg.delay_ab.base_delay_us = 50000;
        cfg.delay_ba.base_delay_us = 100000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("asym_20_40_j10");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 40000;
        cfg.delay_ab.jitter_us = 10000;
        cfg.delay_ba.jitter_us = 10000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("asym_50_150_j10");
        cfg.delay_ab.base_delay_us = 50000;
        cfg.delay_ba.base_delay_us = 150000;
        cfg.delay_ab.jitter_us = 10000;
        cfg.delay_ba.jitter_us = 10000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("asym_80_20_j5");
        cfg.delay_ab.base_delay_us = 80000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }

    // Loss sweep (6)
    {
        ExperimentConfig cfg = BaseConfig("loss_0p5");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.005;
        cfg.loss_ba.loss_rate = 0.005;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("loss_2");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.02;
        cfg.loss_ba.loss_rate = 0.02;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("loss_5");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.05;
        cfg.loss_ba.loss_rate = 0.05;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("loss_10");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.10;
        cfg.loss_ba.loss_rate = 0.10;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("burst_loss_light");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.01;
        cfg.loss_ba.loss_rate = 0.01;
        cfg.loss_ab.burst_start_prob = 0.02;
        cfg.loss_ba.burst_start_prob = 0.02;
        cfg.loss_ab.burst_len_min = 3;
        cfg.loss_ab.burst_len_max = 6;
        cfg.loss_ba.burst_len_min = 3;
        cfg.loss_ba.burst_len_max = 6;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("burst_loss_heavy");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.005;
        cfg.loss_ba.loss_rate = 0.005;
        cfg.loss_ab.burst_start_prob = 0.05;
        cfg.loss_ba.burst_start_prob = 0.05;
        cfg.loss_ab.burst_len_min = 5;
        cfg.loss_ab.burst_len_max = 12;
        cfg.loss_ba.burst_len_min = 5;
        cfg.loss_ba.burst_len_max = 12;
        exps.push_back(cfg);
    }

    // Drift sweep (4)
    {
        ExperimentConfig cfg = BaseConfig("drift_10ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 10.0;
        cfg.drift_ppm_b = -10.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_50ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 50.0;
        cfg.drift_ppm_b = -50.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_100ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 100.0;
        cfg.drift_ppm_b = -100.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_200ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 200.0;
        cfg.drift_ppm_b = -200.0;
        add_with_reverse(cfg, "_bfast");
    }

    // Drift stress (12 + reverse)
    {
        ExperimentConfig cfg = BaseConfig("drift_300ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 300.0;
        cfg.drift_ppm_b = -300.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_500ppm");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 500.0;
        cfg.drift_ppm_b = -500.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_window2s");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        cfg.drift_window_us = 2 * 1000 * 1000ULL;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm_window2s");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        cfg.drift_window_us = 2 * 1000 * 1000ULL;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_sync5s");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.sync_interval_us = 5 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm_sync10s");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.sync_interval_us = 10 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_rate10hz");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.send_rate_hz = 10.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_rate240hz");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.send_rate_hz = 240.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_asym_20_80");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 80000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_asym_80_20");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.base_delay_us = 80000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_loss5");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.loss_ab.loss_rate = 0.05;
        cfg.loss_ba.loss_rate = 0.05;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_spikes");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.delay_ab.spike_prob = 0.05;
        cfg.delay_ba.spike_prob = 0.05;
        cfg.delay_ab.spike_delay_us = 100000;
        cfg.delay_ba.spike_delay_us = 100000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_long300s");
        cfg.duration_us = 300 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm_long300s");
        cfg.duration_us = 300 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        add_with_reverse(cfg, "_bfast");
    }

    // Packet rate + sync interval sweep (6)
    {
        ExperimentConfig cfg = BaseConfig("rate_10hz");
        cfg.send_rate_hz = 10.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("rate_30hz");
        cfg.send_rate_hz = 30.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("rate_120hz");
        cfg.send_rate_hz = 120.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("rate_240hz");
        cfg.send_rate_hz = 240.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("sync_0p5s");
        cfg.sync_interval_us = 500 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("sync_5s");
        cfg.sync_interval_us = 5 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }

    // Spikes / queue / ramp / bimodal (5)
    {
        ExperimentConfig cfg = BaseConfig("spikes_1p_50ms");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.delay_ab.spike_prob = 0.01;
        cfg.delay_ba.spike_prob = 0.01;
        cfg.delay_ab.spike_delay_us = 50000;
        cfg.delay_ba.spike_delay_us = 50000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("spikes_5p_100ms");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.delay_ab.spike_prob = 0.05;
        cfg.delay_ba.spike_prob = 0.05;
        cfg.delay_ab.spike_delay_us = 100000;
        cfg.delay_ba.spike_delay_us = 100000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("queue_sine_50ms_5s");
        cfg.delay_ab.queue_amp_us = 50000;
        cfg.delay_ba.queue_amp_us = 50000;
        cfg.delay_ab.queue_period_us = 5 * 1000 * 1000ULL;
        cfg.delay_ba.queue_period_us = 5 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("ramp_1ms_per_s");
        cfg.delay_ab.ramp_us_per_s = 1000;
        cfg.delay_ba.ramp_us_per_s = 1000;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("bimodal_20_100_20p");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 2000;
        cfg.delay_ba.jitter_us = 2000;
        cfg.delay_ab.bimodal_prob = 0.2;
        cfg.delay_ba.bimodal_prob = 0.2;
        cfg.delay_ab.bimodal_delay_us = 80000;
        cfg.delay_ba.bimodal_delay_us = 80000;
        exps.push_back(cfg);
    }

    // Extremes / stress (6)
    {
        ExperimentConfig cfg = BaseConfig("lan_2ms_j0p2");
        cfg.delay_ab.base_delay_us = 2000;
        cfg.delay_ba.base_delay_us = 2000;
        cfg.delay_ab.jitter_us = 200;
        cfg.delay_ba.jitter_us = 200;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("cellular_stress");
        cfg.delay_ab.base_delay_us = 80000;
        cfg.delay_ba.base_delay_us = 120000;
        cfg.delay_ab.jitter_us = 20000;
        cfg.delay_ba.jitter_us = 20000;
        cfg.loss_ab.loss_rate = 0.02;
        cfg.loss_ba.loss_rate = 0.02;
        cfg.delay_ab.spike_prob = 0.02;
        cfg.delay_ba.spike_prob = 0.02;
        cfg.delay_ab.spike_delay_us = 100000;
        cfg.delay_ba.spike_delay_us = 100000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("satellite_300ms");
        cfg.delay_ab.base_delay_us = 300000;
        cfg.delay_ba.base_delay_us = 300000;
        cfg.delay_ab.jitter_us = 30000;
        cfg.delay_ba.jitter_us = 30000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("short_session_5s");
        cfg.duration_us = 5 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("long_session_120s_drift50");
        cfg.duration_us = 120 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 50.0;
        cfg.drift_ppm_b = -50.0;
        add_with_reverse(cfg, "_bfast");
    }
    {
        ExperimentConfig cfg = BaseConfig("big_offset_30s");
        cfg.offset_b_us = 30 * 1000 * 1000LL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        exps.push_back(cfg);
    }

    return exps;
}

static void AddSeeded(std::vector<ExperimentConfig>& exps, const ExperimentConfig& base, unsigned seeds)
{
    if (seeds <= 1) {
        exps.push_back(base);
        return;
    }
    for (unsigned i = 0; i < seeds; ++i) {
        ExperimentConfig cfg = base;
        char name[128];
        std::snprintf(name, sizeof(name), "%s_s%02u", base.name.c_str(), i + 1);
        cfg.name = name;
        exps.push_back(cfg);
    }
}

static string ToLower(const string& s)
{
    string out = s;
    for (size_t i = 0; i < out.size(); ++i) {
        out[i] = (char)std::tolower((unsigned char)out[i]);
    }
    return out;
}

static void SetSymmetricBase(ExperimentConfig& cfg, double base_ms)
{
    cfg.delay_ab.base_delay_us = (uint32_t)std::llround(base_ms * 1000.0);
    cfg.delay_ba.base_delay_us = cfg.delay_ab.base_delay_us;
}

static void SetAsymmetry(ExperimentConfig& cfg, double base_ms, double asym_ms)
{
    double ab = base_ms - 0.5 * asym_ms;
    double ba = base_ms + 0.5 * asym_ms;
    if (ab < 2.0) {
        ab = 2.0;
        ba = 2.0 + asym_ms;
    }
    cfg.delay_ab.base_delay_us = (uint32_t)std::llround(ab * 1000.0);
    cfg.delay_ba.base_delay_us = (uint32_t)std::llround(ba * 1000.0);
}

static void SetGaussianJitter(DelayModel& model, double rms_ms, double clip_sigma)
{
    model.jitter_mode = JitterMode::Gaussian;
    model.jitter_us = (uint32_t)std::llround(rms_ms * 1000.0);
    model.jitter_clip_sigma = clip_sigma;
}

static void SetUniformJitter(DelayModel& model, double max_ms)
{
    model.jitter_mode = JitterMode::Uniform;
    model.jitter_us = (uint32_t)std::llround(max_ms * 1000.0);
    model.jitter_clip_sigma = 0.0;
}

static void SetLossBoth(ExperimentConfig& cfg, double loss)
{
    cfg.loss_ab.loss_rate = loss;
    cfg.loss_ba.loss_rate = loss;
}

static void SetDriftOpposite(ExperimentConfig& cfg, double ppm)
{
    cfg.drift_ppm_a = ppm;
    cfg.drift_ppm_b = -ppm;
}

static std::vector<ExperimentConfig> BuildTestPlan()
{
    std::vector<ExperimentConfig> exps;
    const uint64_t kSecond = 1000000ULL;
    const uint64_t kShort = 10 * kSecond;
    const uint64_t kMedium = 30 * kSecond;
    const uint64_t kLong = 120 * kSecond;
    const int64_t ts23_half_range_us = (int64_t)1 << (23 + kTime23LostBits);

    auto add = [&](const ExperimentConfig& cfg) {
        exps.push_back(cfg);
    };

    // Group B — Synchronization state machine & MinDelta exchange
    {
        ExperimentConfig cfg = BaseConfig("UT-B01_initial_state");
        cfg.duration_us = 5 * kSecond;
        cfg.send_rate_hz = 0.0;
        cfg.sync_interval_us = 0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B02_no_mindelta");
        cfg.duration_us = kShort;
        cfg.sync_interval_us = 0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B03_mindelta_only");
        cfg.duration_us = kShort;
        cfg.send_rate_hz = 0.0;
        cfg.sync_interval_us = kSecond;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B04_happy_path");
        cfg.duration_us = kShort;
        SetSymmetricBase(cfg, 20.0);
        SetGaussianJitter(cfg.delay_ab, 0.5, 4.0);
        SetGaussianJitter(cfg.delay_ba, 0.5, 4.0);
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B05_mindelta_reorder");
        cfg.duration_us = kShort;
        cfg.reorder_prob_ab = 0.5;
        cfg.reorder_prob_ba = 0.5;
        cfg.reorder_advance_us = 20000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B06_mindelta_duplicate");
        cfg.duration_us = kShort;
        cfg.duplicate_prob_ab = 0.2;
        cfg.duplicate_prob_ba = 0.2;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B07_mindelta_loss");
        cfg.duration_us = kMedium;
        cfg.use_sync_loss = true;
        cfg.loss_sync_ab.loss_rate = 0.9;
        cfg.loss_sync_ba.loss_rate = 0.9;
        cfg.loss_ab.loss_rate = 0.0;
        cfg.loss_ba.loss_rate = 0.0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B08_mindelta_delay_spike");
        cfg.duration_us = kMedium;
        cfg.delay_ab.spike_prob = 0.1;
        cfg.delay_ba.spike_prob = 0.1;
        cfg.delay_ab.spike_delay_us = 5 * kSecond;
        cfg.delay_ba.spike_delay_us = 5 * kSecond;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B09_asym_sync_interval");
        cfg.duration_us = kMedium;
        cfg.sync_interval_ab_us = 500000;
        cfg.sync_interval_ba_us = 10 * kSecond;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-B10_resync_after_silence");
        cfg.duration_us = kLong;
        cfg.blackout_start_us = 30 * kSecond;
        cfg.blackout_end_us = 90 * kSecond;
        add(cfg);
    }

    // Group C — OWD and minimum-OWD behavior
    {
        ExperimentConfig cfg = BaseConfig("UT-C01_owd_zero_unsynced");
        cfg.duration_us = kShort;
        cfg.sync_interval_us = 0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C02_constant_delay");
        cfg.duration_us = kShort;
        SetSymmetricBase(cfg, 20.0);
        cfg.delay_ab.jitter_us = 0;
        cfg.delay_ba.jitter_us = 0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C03_min_owd_match");
        cfg.duration_us = kShort;
        SetSymmetricBase(cfg, 30.0);
        SetGaussianJitter(cfg.delay_ab, 2.0, 0.0);
        SetGaussianJitter(cfg.delay_ba, 2.0, 0.0);
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C04_queue_spikes");
        cfg.duration_us = kShort;
        cfg.delay_ab.spike_prob = 0.05;
        cfg.delay_ba.spike_prob = 0.05;
        cfg.delay_ab.spike_delay_us = 100000;
        cfg.delay_ba.spike_delay_us = 100000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C05_random_walk_queue");
        cfg.duration_us = kMedium;
        cfg.delay_ab.rw_step_interval_us = 100000;
        cfg.delay_ba.rw_step_interval_us = 100000;
        cfg.delay_ab.rw_step_us = 2000;
        cfg.delay_ba.rw_step_us = 2000;
        cfg.delay_ab.rw_max_us = 100000;
        cfg.delay_ba.rw_max_us = 100000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C06_step_change");
        cfg.duration_us = kMedium;
        cfg.delay_ab.step_at_us = 30 * kSecond;
        cfg.delay_ba.step_at_us = 30 * kSecond;
        cfg.delay_ab.step_delta_us = 50000;
        cfg.delay_ba.step_delta_us = 50000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C07_periodic_delay");
        cfg.duration_us = kMedium;
        cfg.delay_ab.queue_amp_us = 20000;
        cfg.delay_ba.queue_amp_us = 20000;
        cfg.delay_ab.queue_period_us = 10 * kSecond;
        cfg.delay_ba.queue_period_us = 10 * kSecond;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C08_severe_asymmetry");
        cfg.duration_us = kShort;
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 120000;
        cfg.delay_ab.jitter_us = 0;
        cfg.delay_ba.jitter_us = 0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C09_heavy_tail");
        cfg.duration_us = kShort;
        cfg.delay_ab.jitter_mode = JitterMode::Pareto;
        cfg.delay_ba.jitter_mode = JitterMode::Pareto;
        cfg.delay_ab.pareto_alpha = 2.0;
        cfg.delay_ba.pareto_alpha = 2.0;
        cfg.delay_ab.pareto_scale_us = 1000;
        cfg.delay_ba.pareto_scale_us = 1000;
        cfg.delay_ab.spike_prob = 0.01;
        cfg.delay_ba.spike_prob = 0.01;
        cfg.delay_ab.spike_delay_us = 300000;
        cfg.delay_ba.spike_delay_us = 300000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-C10_extreme_late_packet");
        cfg.duration_us = kMedium;
        cfg.inject_late_ab = true;
        cfg.inject_late_at_us = 10 * kSecond;
        cfg.inject_late_delay_us = 9 * kSecond;
        add(cfg);
    }

    // Group D — Loss/reorder/dup robustness
    {
        ExperimentConfig cfg = BaseConfig("UT-D01_loss_1p");
        cfg.duration_us = kShort;
        SetLossBoth(cfg, 0.01);
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D02_loss_10p");
        cfg.duration_us = kShort;
        SetLossBoth(cfg, 0.10);
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D03_burst_loss");
        cfg.duration_us = kMedium;
        cfg.loss_ab.burst_start_prob = 0.02;
        cfg.loss_ba.burst_start_prob = 0.02;
        cfg.loss_ab.burst_len_min = 10;
        cfg.loss_ab.burst_len_max = 40;
        cfg.loss_ba.burst_len_min = 10;
        cfg.loss_ba.burst_len_max = 40;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D04_periodic_loss");
        cfg.duration_us = kShort;
        cfg.loss_ab.periodic_n = 10;
        cfg.loss_ba.periodic_n = 10;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D05_delay_correlated_loss");
        cfg.duration_us = kMedium;
        cfg.delay_ab.jitter_us = 10000;
        cfg.delay_ba.jitter_us = 10000;
        cfg.loss_ab.delay_drop_threshold_us = 60000;
        cfg.loss_ba.delay_drop_threshold_us = 60000;
        cfg.loss_ab.delay_drop_prob = 1.0;
        cfg.loss_ba.delay_drop_prob = 1.0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D06_directional_loss");
        cfg.duration_us = kShort;
        cfg.loss_ab.loss_rate = 0.01;
        cfg.loss_ba.loss_rate = 0.20;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D07_reorder_1p");
        cfg.duration_us = kShort;
        cfg.reorder_prob_ab = 0.01;
        cfg.reorder_prob_ba = 0.01;
        cfg.reorder_delay_us = 50000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D08_reorder_10p");
        cfg.duration_us = kShort;
        cfg.reorder_prob_ab = 0.10;
        cfg.reorder_prob_ba = 0.10;
        cfg.reorder_delay_us = 200000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D09_duplicates_0p1");
        cfg.duration_us = kShort;
        cfg.duplicate_prob_ab = 0.001;
        cfg.duplicate_prob_ba = 0.001;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D10_dup_1p_reorder_5p");
        cfg.duration_us = kShort;
        cfg.duplicate_prob_ab = 0.01;
        cfg.duplicate_prob_ba = 0.01;
        cfg.reorder_prob_ab = 0.05;
        cfg.reorder_prob_ba = 0.05;
        cfg.reorder_delay_us = 100000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D11_mindelta_dropped");
        cfg.duration_us = kShort;
        cfg.use_sync_loss = true;
        cfg.loss_sync_ab.loss_rate = 1.0;
        cfg.loss_sync_ba.loss_rate = 1.0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-D12_mindelta_reorder_dup");
        cfg.duration_us = kShort;
        cfg.reorder_prob_ab = 0.10;
        cfg.reorder_prob_ba = 0.10;
        cfg.reorder_delay_us = 100000;
        cfg.duplicate_prob_ab = 0.05;
        cfg.duplicate_prob_ba = 0.05;
        add(cfg);
    }

    // Group E — Clock offset and skew/drift
    {
        const int64_t offsets[] = {0, 1000, -1000, 100000, -100000, 1000000, -1000000, 4000000, -4000000, 7500000, -7500000};
        for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
            ExperimentConfig cfg = BaseConfig("UT-E01_offset");
            cfg.duration_us = kShort;
            cfg.offset_b_us = offsets[i];
            char name[64];
            std::snprintf(name, sizeof(name), "UT-E01_offset_%+lldus", (long long)offsets[i]);
            cfg.name = name;
            add(cfg);
        }
    }
    {
        const int64_t near_boundary = ts23_half_range_us - 200000;
        const int64_t offsets[] = {near_boundary, -near_boundary};
        for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
            ExperimentConfig cfg = BaseConfig("UT-E02_near_boundary");
            cfg.duration_us = kShort;
            cfg.offset_b_us = offsets[i];
            char name[64];
            std::snprintf(name, sizeof(name), "UT-E02_offset_%+lldus", (long long)offsets[i]);
            cfg.name = name;
            add(cfg);
        }
    }
    {
        const int64_t offsets[] = {20000000LL, -20000000LL, 3600000000LL, -3600000000LL};
        for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
            ExperimentConfig cfg = BaseConfig("UT-E03_offset_outside");
            cfg.duration_us = kShort;
            cfg.offset_b_us = offsets[i];
            char name[72];
            std::snprintf(name, sizeof(name), "UT-E03_offset_%+lldus", (long long)offsets[i]);
            cfg.name = name;
            add(cfg);
        }
    }
    {
        const double skews[] = {0, 10, 50, 100, 200, 500, 1000, 5000};
        for (size_t i = 0; i < sizeof(skews) / sizeof(skews[0]); ++i) {
            const double skew = skews[i];
            const int sign_count = (skew == 0.0) ? 1 : 2;
            for (int s = 0; s < sign_count; ++s) {
                const double drift = (s == 0) ? skew : -skew;
                ExperimentConfig cfg = BaseConfig("UT-E04_skew");
                cfg.duration_us = kMedium;
                cfg.drift_ppm_a = drift;
                cfg.drift_ppm_b = -drift;
                char name[64];
                std::snprintf(name, sizeof(name), "UT-E04_skew_%+.0fppm", drift);
                cfg.name = name;
                add(cfg);
            }
        }
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E05_skew_opposite");
        cfg.duration_us = kMedium;
        cfg.drift_ppm_a = 200.0;
        cfg.drift_ppm_b = -200.0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E06_skew_step");
        cfg.duration_us = kMedium;
        cfg.drift_ppm_a = 50.0;
        cfg.drift_ppm_b = -50.0;
        cfg.drift_step_enabled = true;
        cfg.drift_step_time_us = 30 * kSecond;
        cfg.drift_step_delta_ppm_a = 150.0;
        cfg.drift_step_delta_ppm_b = -150.0;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E07_sine_wander");
        cfg.duration_us = kMedium;
        cfg.drift_sine_amp_ppm = 100.0;
        cfg.drift_sine_period_us = 60 * kSecond;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E08_rw_wander");
        cfg.duration_us = kMedium;
        cfg.drift_rw_step_ppm = 5.0;
        cfg.drift_rw_step_interval_us = kSecond;
        add(cfg);
    }
    {
        const uint32_t quants[] = {1, 100, 1000};
        for (size_t i = 0; i < sizeof(quants) / sizeof(quants[0]); ++i) {
            ExperimentConfig cfg = BaseConfig("UT-E09_quantize");
            cfg.duration_us = kShort;
            cfg.quantize_us = quants[i];
            char name[64];
            std::snprintf(name, sizeof(name), "UT-E09_quantize_%uus", quants[i]);
            cfg.name = name;
            add(cfg);
        }
    }
    {
        const uint32_t noises[] = {0, 50, 200, 2000};
        for (size_t i = 0; i < sizeof(noises) / sizeof(noises[0]); ++i) {
            ExperimentConfig cfg = BaseConfig("UT-E10_recv_noise");
            cfg.duration_us = kShort;
            cfg.recv_noise_us = noises[i];
            cfg.recv_noise_mode = NoiseMode::Uniform;
            char name[64];
            std::snprintf(name, sizeof(name), "UT-E10_noise_%uus", noises[i]);
            cfg.name = name;
            add(cfg);
        }
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E11_clock_step_forward");
        cfg.duration_us = kMedium;
        cfg.clock_step_enabled = true;
        cfg.clock_step_time_us = 10 * kSecond;
        cfg.clock_step_b_us = 200000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E12_clock_step_backward");
        cfg.duration_us = kMedium;
        cfg.clock_step_enabled = true;
        cfg.clock_step_time_us = 10 * kSecond;
        cfg.clock_step_b_us = -20000;
        add(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E13_clock_step_opposite");
        cfg.duration_us = kMedium;
        cfg.clock_step_enabled = true;
        cfg.clock_step_time_us = 10 * kSecond;
        cfg.clock_step_a_us = 100000;
        cfg.clock_step_b_us = -100000;
        add(cfg);
    }
    {
        struct StepCase {
            const char* tag;
            int64_t step_us;
            uint64_t duration_us;
            uint64_t step_time_us;
        };
        const StepCase cases[] = {
            {"small", 2000000LL, kMedium, 10 * kSecond},
            {"large", 30000000LL, kLong, 40 * kSecond},
        };
        for (size_t ci = 0; ci < sizeof(cases) / sizeof(cases[0]); ++ci) {
            const StepCase& sc = cases[ci];
            const int64_t steps[] = {sc.step_us, -sc.step_us};
            for (int si = 0; si < 2; ++si) {
                const char* dir = (steps[si] >= 0) ? "fwd" : "back";
                {
                    ExperimentConfig cfg = BaseConfig("UT-E15_clock_step");
                    cfg.duration_us = sc.duration_us;
                    cfg.clock_step_enabled = true;
                    cfg.clock_step_time_us = sc.step_time_us;
                    cfg.clock_step_a_us = steps[si];
                    cfg.clock_step_b_us = 0;
                    char name[96];
                    std::snprintf(name, sizeof(name),
                        "UT-E15_clock_step_%s_a_%s", sc.tag, dir);
                    cfg.name = name;
                    add(cfg);
                }
                {
                    ExperimentConfig cfg = BaseConfig("UT-E16_clock_step");
                    cfg.duration_us = sc.duration_us;
                    cfg.clock_step_enabled = true;
                    cfg.clock_step_time_us = sc.step_time_us;
                    cfg.clock_step_a_us = 0;
                    cfg.clock_step_b_us = steps[si];
                    char name[96];
                    std::snprintf(name, sizeof(name),
                        "UT-E16_clock_step_%s_b_%s", sc.tag, dir);
                    cfg.name = name;
                    add(cfg);
                }
            }
        }
    }
    {
        ExperimentConfig cfg = BaseConfig("UT-E14_high_skew_low_rate");
        cfg.duration_us = kMedium;
        cfg.send_rate_hz = 2.0;
        cfg.drift_ppm_a = 500.0;
        cfg.drift_ppm_b = -500.0;
        add(cfg);
    }

    return exps;
}

static std::vector<ExperimentConfig> BuildMonteCarloFamily(const string& family, unsigned seeds)
{
    std::vector<ExperimentConfig> exps;
    const string key = ToLower(family);
    const uint64_t kSecond = 1000000ULL;

    if (key == "all") {
        const char* families[] = {
            "f01","f02","f03","f04","f05","f06","f07","f08","f09","f10",
            "f11","f12","f13","f14","f15","f16","f17","f18","f19","f20",
            "f21","f22"
        };
        for (size_t i = 0; i < sizeof(families) / sizeof(families[0]); ++i) {
            std::vector<ExperimentConfig> sub = BuildMonteCarloFamily(families[i], seeds);
            exps.insert(exps.end(), sub.begin(), sub.end());
        }
        return exps;
    }

    auto add = [&](const ExperimentConfig& cfg) {
        AddSeeded(exps, cfg, seeds);
    };

    if (key == "f01" || key == "mc-f01" || key == "mc_f01" || key == "mcf01") {
        const double bases[] = {10, 30, 80, 150};
        const double jitters[] = {0, 0.5, 2, 5, 10};
        const double losses[] = {0.0, 0.01, 0.05};
        const double skews[] = {0, 50, 200};
        for (size_t bi = 0; bi < sizeof(bases)/sizeof(bases[0]); ++bi) {
            for (size_t ji = 0; ji < sizeof(jitters)/sizeof(jitters[0]); ++ji) {
                for (size_t li = 0; li < sizeof(losses)/sizeof(losses[0]); ++li) {
                    for (size_t si = 0; si < sizeof(skews)/sizeof(skews[0]); ++si) {
                        const double skew = skews[si];
                        const int sign_count = (skew == 0.0) ? 1 : 2;
                        for (int s = 0; s < sign_count; ++s) {
                            const double drift = (s == 0) ? skew : -skew;
                            ExperimentConfig cfg = BaseConfig("MC-F01");
                            cfg.duration_us = 30 * kSecond;
                            SetSymmetricBase(cfg, bases[bi]);
                            SetGaussianJitter(cfg.delay_ab, jitters[ji], 4.0);
                            SetGaussianJitter(cfg.delay_ba, jitters[ji], 4.0);
                            SetLossBoth(cfg, losses[li]);
                            cfg.drift_ppm_a = drift;
                            cfg.drift_ppm_b = -drift;
                            char name[128];
                            std::snprintf(
                                name, sizeof(name),
                                "MC-F01_b%.0f_j%.1f_loss%.0f_skew%+.0f",
                                bases[bi], jitters[ji], losses[li] * 100.0, drift);
                            cfg.name = name;
                            add(cfg);
                        }
                    }
                }
            }
        }
        return exps;
    }

    if (key == "f02" || key == "mc-f02" || key == "mc_f02" || key == "mcf02") {
        const double asymms[] = {0, 10, 30, 60, 120};
        const double jitters[] = {0, 2, 10};
        const double losses[] = {0.0, 0.01, 0.10};
        const double skews[] = {0, 100};
        for (size_t ai = 0; ai < sizeof(asymms)/sizeof(asymms[0]); ++ai) {
            for (size_t ji = 0; ji < sizeof(jitters)/sizeof(jitters[0]); ++ji) {
                for (size_t li = 0; li < sizeof(losses)/sizeof(losses[0]); ++li) {
                    for (size_t si = 0; si < sizeof(skews)/sizeof(skews[0]); ++si) {
                        const double skew = skews[si];
                        const int sign_count = (skew == 0.0) ? 1 : 2;
                        for (int s = 0; s < sign_count; ++s) {
                            const double drift = (s == 0) ? skew : -skew;
                            ExperimentConfig cfg = BaseConfig("MC-F02");
                            cfg.duration_us = 30 * kSecond;
                            SetAsymmetry(cfg, 30.0, asymms[ai]);
                            SetGaussianJitter(cfg.delay_ab, jitters[ji], 4.0);
                            SetGaussianJitter(cfg.delay_ba, jitters[ji], 4.0);
                            SetLossBoth(cfg, losses[li]);
                            cfg.drift_ppm_a = drift;
                            cfg.drift_ppm_b = -drift;
                            char name[128];
                            std::snprintf(
                                name, sizeof(name),
                                "MC-F02_asym%.0f_j%.0f_loss%.0f_skew%+.0f",
                                asymms[ai], jitters[ji], losses[li] * 100.0, drift);
                            cfg.name = name;
                            add(cfg);
                        }
                    }
                }
            }
        }
        return exps;
    }

    if (key == "f03" || key == "mc-f03" || key == "mc_f03" || key == "mcf03") {
        const double alphas[] = {1.5, 2.0, 3.0};
        const double medians[] = {0.5, 2.0};
        const double losses[] = {0.0, 0.01, 0.05};
        for (size_t ai = 0; ai < sizeof(alphas)/sizeof(alphas[0]); ++ai) {
            for (size_t mi = 0; mi < sizeof(medians)/sizeof(medians[0]); ++mi) {
                for (size_t li = 0; li < sizeof(losses)/sizeof(losses[0]); ++li) {
                    ExperimentConfig cfg = BaseConfig("MC-F03");
                    cfg.duration_us = 30 * kSecond;
                    cfg.delay_ab.jitter_mode = JitterMode::Pareto;
                    cfg.delay_ba.jitter_mode = JitterMode::Pareto;
                    cfg.delay_ab.pareto_alpha = alphas[ai];
                    cfg.delay_ba.pareto_alpha = alphas[ai];
                    const double median_us = medians[mi] * 1000.0;
                    const double scale = median_us / std::pow(2.0, 1.0 / alphas[ai]);
                    cfg.delay_ab.pareto_scale_us = (uint32_t)std::llround(scale);
                    cfg.delay_ba.pareto_scale_us = (uint32_t)std::llround(scale);
                    cfg.delay_ab.spike_prob = 0.01;
                    cfg.delay_ba.spike_prob = 0.01;
                    cfg.delay_ab.spike_delay_us = 200000;
                    cfg.delay_ba.spike_delay_us = 200000;
                    SetLossBoth(cfg, losses[li]);
                    char name[128];
                    std::snprintf(
                        name, sizeof(name),
                        "MC-F03_alpha%.1f_med%.1f_loss%.0f",
                        alphas[ai], medians[mi], losses[li] * 100.0);
                    cfg.name = name;
                    add(cfg);
                }
            }
        }
        return exps;
    }

    if (key == "f04" || key == "mc-f04" || key == "mc_f04" || key == "mcf04") {
        const double high_delays_ms[] = {8, 20};
        const double losses[] = {0.0, 0.01, 0.10};
        const double reorders[] = {0.0, 0.02};
        for (size_t hi = 0; hi < sizeof(high_delays_ms)/sizeof(high_delays_ms[0]); ++hi) {
            for (size_t li = 0; li < sizeof(losses)/sizeof(losses[0]); ++li) {
                for (size_t ri = 0; ri < sizeof(reorders)/sizeof(reorders[0]); ++ri) {
                    ExperimentConfig cfg = BaseConfig("MC-F04");
                    cfg.duration_us = 30 * kSecond;
                    SetSymmetricBase(cfg, 50.0);
                    SetGaussianJitter(cfg.delay_ab, 0.5, 4.0);
                    SetGaussianJitter(cfg.delay_ba, 0.5, 4.0);
                    cfg.delay_ab.bimodal_prob = 0.2;
                    cfg.delay_ba.bimodal_prob = 0.2;
                    cfg.delay_ab.bimodal_delay_us = (uint32_t)std::llround(high_delays_ms[hi] * 1000.0);
                    cfg.delay_ba.bimodal_delay_us = cfg.delay_ab.bimodal_delay_us;
                    SetLossBoth(cfg, losses[li]);
                    cfg.reorder_prob_ab = reorders[ri];
                    cfg.reorder_prob_ba = reorders[ri];
                    char name[128];
                    std::snprintf(
                        name, sizeof(name),
                        "MC-F04_hi%.0f_loss%.0f_reorder%.0f",
                        high_delays_ms[hi], losses[li] * 100.0, reorders[ri] * 100.0);
                    cfg.name = name;
                    add(cfg);
                }
            }
        }
        return exps;
    }

    if (key == "f05" || key == "mc-f05" || key == "mc_f05" || key == "mcf05") {
        const double qmax_ms[] = {20, 100, 300};
        for (size_t qi = 0; qi < sizeof(qmax_ms)/sizeof(qmax_ms[0]); ++qi) {
            ExperimentConfig cfg = BaseConfig("MC-F05");
            cfg.duration_us = 60 * kSecond;
            SetSymmetricBase(cfg, 50.0);
            SetUniformJitter(cfg.delay_ab, 1.0);
            SetUniformJitter(cfg.delay_ba, 1.0);
            cfg.delay_ab.rw_step_interval_us = 10000;
            cfg.delay_ba.rw_step_interval_us = 10000;
            cfg.delay_ab.rw_step_us = (int32_t)std::llround(qmax_ms[qi] * 1000.0 / 20.0);
            cfg.delay_ba.rw_step_us = cfg.delay_ab.rw_step_us;
            cfg.delay_ab.rw_max_us = (uint32_t)std::llround(qmax_ms[qi] * 1000.0);
            cfg.delay_ba.rw_max_us = cfg.delay_ab.rw_max_us;
            const uint32_t threshold = (uint32_t)std::llround(50000.0 + 0.8 * qmax_ms[qi] * 1000.0);
            cfg.loss_ab.delay_drop_threshold_us = threshold;
            cfg.loss_ba.delay_drop_threshold_us = threshold;
            cfg.loss_ab.delay_drop_prob = 1.0;
            cfg.loss_ba.delay_drop_prob = 1.0;
            char name[128];
            std::snprintf(name, sizeof(name), "MC-F05_qmax%.0f", qmax_ms[qi]);
            cfg.name = name;
            add(cfg);
        }
        return exps;
    }

    if (key == "f06" || key == "mc-f06" || key == "mc_f06" || key == "mcf06") {
        const double up_ms[] = {20, 50, 100};
        const double down_ms[] = {20, 50};
        for (size_t ui = 0; ui < sizeof(up_ms)/sizeof(up_ms[0]); ++ui) {
            for (size_t di = 0; di < sizeof(down_ms)/sizeof(down_ms[0]); ++di) {
                ExperimentConfig cfg = BaseConfig("MC-F06");
                cfg.duration_us = 90 * kSecond;
                SetSymmetricBase(cfg, 50.0);
                SetGaussianJitter(cfg.delay_ab, 2.0, 4.0);
                SetGaussianJitter(cfg.delay_ba, 2.0, 4.0);
                SetLossBoth(cfg, 0.01);
                cfg.delay_ab.step_at_us = 30 * kSecond;
                cfg.delay_ba.step_at_us = 30 * kSecond;
                cfg.delay_ab.step_delta_us = (int32_t)std::llround(up_ms[ui] * 1000.0);
                cfg.delay_ba.step_delta_us = cfg.delay_ab.step_delta_us;
                cfg.delay_ab.step2_at_us = 60 * kSecond;
                cfg.delay_ba.step2_at_us = 60 * kSecond;
                cfg.delay_ab.step2_delta_us = -(int32_t)std::llround(down_ms[di] * 1000.0);
                cfg.delay_ba.step2_delta_us = cfg.delay_ab.step2_delta_us;
                char name[128];
                std::snprintf(name, sizeof(name), "MC-F06_up%.0f_down%.0f", up_ms[ui], down_ms[di]);
                cfg.name = name;
                add(cfg);
            }
        }
        return exps;
    }

    if (key == "f07" || key == "mc-f07" || key == "mc_f07" || key == "mcf07") {
        const double jitters[] = {2, 10};
        for (size_t ji = 0; ji < sizeof(jitters)/sizeof(jitters[0]); ++ji) {
            for (int sign = -1; sign <= 1; sign += 2) {
                ExperimentConfig cfg = BaseConfig("MC-F07");
                cfg.duration_us = 90 * kSecond;
                SetSymmetricBase(cfg, 50.0);
                SetGaussianJitter(cfg.delay_ab, jitters[ji], 4.0);
                SetGaussianJitter(cfg.delay_ba, jitters[ji], 4.0);
                cfg.delay_ab.ramp_us_per_s = (int32_t)std::llround(100000.0 / 60.0);
                cfg.delay_ba.ramp_us_per_s = cfg.delay_ab.ramp_us_per_s;
                cfg.drift_ppm_a = 100.0 * sign;
                cfg.drift_ppm_b = -cfg.drift_ppm_a;
                char name[128];
                std::snprintf(name, sizeof(name), "MC-F07_j%.0f_skew%+.0f", jitters[ji], cfg.drift_ppm_a);
                cfg.name = name;
                add(cfg);
            }
        }
        return exps;
    }

    if (key == "f08" || key == "mc-f08" || key == "mc_f08" || key == "mcf08") {
        const double qmax_ms[] = {50, 200};
        const double periods_s[] = {1, 5, 10};
        for (size_t qi = 0; qi < sizeof(qmax_ms)/sizeof(qmax_ms[0]); ++qi) {
            for (size_t pi = 0; pi < sizeof(periods_s)/sizeof(periods_s[0]); ++pi) {
                ExperimentConfig cfg = BaseConfig("MC-F08");
                cfg.duration_us = 60 * kSecond;
                SetSymmetricBase(cfg, 50.0);
                SetGaussianJitter(cfg.delay_ab, 1.0, 4.0);
                SetGaussianJitter(cfg.delay_ba, 1.0, 4.0);
                cfg.delay_ab.saw_amp_us = (uint32_t)std::llround(qmax_ms[qi] * 1000.0);
                cfg.delay_ba.saw_amp_us = cfg.delay_ab.saw_amp_us;
                cfg.delay_ab.saw_period_us = (uint64_t)std::llround(periods_s[pi] * 1000000.0);
                cfg.delay_ba.saw_period_us = cfg.delay_ab.saw_period_us;
                char name[128];
                std::snprintf(name, sizeof(name), "MC-F08_q%.0f_p%.0f", qmax_ms[qi], periods_s[pi]);
                cfg.name = name;
                add(cfg);
            }
        }
        return exps;
    }

    if (key == "f09" || key == "mc-f09" || key == "mc_f09" || key == "mcf09") {
        const double losses[] = {0.01, 0.05, 0.10, 0.20};
        for (size_t li = 0; li < sizeof(losses)/sizeof(losses[0]); ++li) {
            const double loss = losses[li];
            {
                ExperimentConfig cfg = BaseConfig("MC-F09_iid");
                cfg.duration_us = 30 * kSecond;
                SetLossBoth(cfg, loss);
                char name[96];
                std::snprintf(name, sizeof(name), "MC-F09_iid_loss%.0f", loss * 100.0);
                cfg.name = name;
                add(cfg);
            }
            {
                ExperimentConfig cfg = BaseConfig("MC-F09_burst");
                cfg.duration_us = 30 * kSecond;
                cfg.loss_ab.burst_start_prob = loss / 5.0;
                cfg.loss_ba.burst_start_prob = loss / 5.0;
                cfg.loss_ab.burst_len_min = 5;
                cfg.loss_ab.burst_len_max = 20;
                cfg.loss_ba.burst_len_min = 5;
                cfg.loss_ba.burst_len_max = 20;
                char name[96];
                std::snprintf(name, sizeof(name), "MC-F09_burst_loss%.0f", loss * 100.0);
                cfg.name = name;
                add(cfg);
            }
            {
                ExperimentConfig cfg = BaseConfig("MC-F09_periodic");
                cfg.duration_us = 30 * kSecond;
                const uint32_t period = (uint32_t)std::llround(1.0 / loss);
                cfg.loss_ab.periodic_n = period;
                cfg.loss_ba.periodic_n = period;
                char name[96];
                std::snprintf(name, sizeof(name), "MC-F09_periodic_loss%.0f", loss * 100.0);
                cfg.name = name;
                add(cfg);
            }
            {
                ExperimentConfig cfg = BaseConfig("MC-F09_delay_corr");
                cfg.duration_us = 30 * kSecond;
                cfg.delay_ab.jitter_us = 10000;
                cfg.delay_ba.jitter_us = 10000;
                cfg.loss_ab.delay_drop_threshold_us = 60000;
                cfg.loss_ba.delay_drop_threshold_us = 60000;
                cfg.loss_ab.delay_drop_prob = 1.0;
                cfg.loss_ba.delay_drop_prob = 1.0;
                char name[96];
                std::snprintf(name, sizeof(name), "MC-F09_delaycorr_loss%.0f", loss * 100.0);
                cfg.name = name;
                add(cfg);
            }
        }
        return exps;
    }

    if (key == "f10" || key == "mc-f10" || key == "mc_f10" || key == "mcf10") {
        const double reorders[] = {0.0, 0.01, 0.05, 0.10};
        const int severities[] = {1, 5, 20};
        for (size_t ri = 0; ri < sizeof(reorders)/sizeof(reorders[0]); ++ri) {
            for (size_t si = 0; si < sizeof(severities)/sizeof(severities[0]); ++si) {
                ExperimentConfig cfg = BaseConfig("MC-F10");
                cfg.duration_us = 30 * kSecond;
                cfg.send_rate_hz = 60.0;
                const uint32_t interval_us = (uint32_t)std::llround(1000000.0 / cfg.send_rate_hz);
                cfg.reorder_prob_ab = reorders[ri];
                cfg.reorder_prob_ba = reorders[ri];
                cfg.reorder_delay_us = interval_us * (uint32_t)severities[si];
                SetLossBoth(cfg, 0.01);
                char name[128];
                std::snprintf(name, sizeof(name), "MC-F10_reorder%.0f_sev%d", reorders[ri] * 100.0, severities[si]);
                cfg.name = name;
                add(cfg);
            }
        }
        return exps;
    }

    if (key == "f11" || key == "mc-f11" || key == "mc_f11" || key == "mcf11") {
        const double dups[] = {0.0, 0.001, 0.01, 0.05};
        for (size_t di = 0; di < sizeof(dups)/sizeof(dups[0]); ++di) {
            ExperimentConfig cfg = BaseConfig("MC-F11");
            cfg.duration_us = 30 * kSecond;
            cfg.duplicate_prob_ab = dups[di];
            cfg.duplicate_prob_ba = dups[di];
            cfg.reorder_prob_ab = 0.02;
            cfg.reorder_prob_ba = 0.02;
            char name[96];
            std::snprintf(name, sizeof(name), "MC-F11_dup%.2f", dups[di] * 100.0);
            cfg.name = name;
            add(cfg);
        }
        return exps;
    }

    if (key == "f12" || key == "mc-f12" || key == "mc_f12" || key == "mcf12") {
        const double skews[] = {0, 10, 50, 100, 200, 500, 1000};
        const double rates[] = {10, 60, 200};
        const double jitters[] = {0.5, 5};
        for (size_t si = 0; si < sizeof(skews)/sizeof(skews[0]); ++si) {
            const double skew = skews[si];
            const int sign_count = (skew == 0.0) ? 1 : 2;
            for (int s = 0; s < sign_count; ++s) {
                const double drift = (s == 0) ? skew : -skew;
                for (size_t ri = 0; ri < sizeof(rates)/sizeof(rates[0]); ++ri) {
                    for (size_t ji = 0; ji < sizeof(jitters)/sizeof(jitters[0]); ++ji) {
                        ExperimentConfig cfg = BaseConfig("MC-F12");
                        cfg.duration_us = 30 * kSecond;
                        cfg.send_rate_hz = rates[ri];
                        SetGaussianJitter(cfg.delay_ab, jitters[ji], 4.0);
                        SetGaussianJitter(cfg.delay_ba, jitters[ji], 4.0);
                        cfg.drift_ppm_a = drift;
                        cfg.drift_ppm_b = -drift;
                        char name[128];
                        std::snprintf(name, sizeof(name), "MC-F12_skew%+.0f_rate%.0f_j%.1f", drift, rates[ri], jitters[ji]);
                        cfg.name = name;
                        add(cfg);
                    }
                }
            }
        }
        return exps;
    }

    if (key == "f13" || key == "mc-f13" || key == "mc_f13" || key == "mcf13") {
        const double amps[] = {100, 200, 300};
        const double periods[] = {30, 60, 120};
        for (size_t ai = 0; ai < sizeof(amps)/sizeof(amps[0]); ++ai) {
            for (size_t pi = 0; pi < sizeof(periods)/sizeof(periods[0]); ++pi) {
                ExperimentConfig cfg = BaseConfig("MC-F13_sine");
                cfg.duration_us = 120 * kSecond;
                cfg.drift_sine_amp_ppm = amps[ai];
                cfg.drift_sine_period_us = (uint64_t)std::llround(periods[pi] * 1000000.0);
                SetGaussianJitter(cfg.delay_ab, 5.0, 4.0);
                SetGaussianJitter(cfg.delay_ba, 5.0, 4.0);
                char name[128];
                std::snprintf(name, sizeof(name), "MC-F13_sine_amp%.0f_p%.0f", amps[ai], periods[pi]);
                cfg.name = name;
                add(cfg);
            }
        }
        const double rw_steps[] = {1, 5, 20};
        for (size_t ri = 0; ri < sizeof(rw_steps)/sizeof(rw_steps[0]); ++ri) {
            ExperimentConfig cfg = BaseConfig("MC-F13_rw");
            cfg.duration_us = 120 * kSecond;
            cfg.drift_rw_step_ppm = rw_steps[ri];
            cfg.drift_rw_step_interval_us = kSecond;
            SetGaussianJitter(cfg.delay_ab, 5.0, 4.0);
            SetGaussianJitter(cfg.delay_ba, 5.0, 4.0);
            char name[128];
            std::snprintf(name, sizeof(name), "MC-F13_rw_step%.0f", rw_steps[ri]);
            cfg.name = name;
            add(cfg);
        }
        return exps;
    }

    if (key == "f14" || key == "mc-f14" || key == "mc_f14" || key == "mcf14") {
        const uint32_t noises[] = {200, 2000};
        for (size_t ni = 0; ni < sizeof(noises)/sizeof(noises[0]); ++ni) {
            ExperimentConfig cfg = BaseConfig("MC-F14_uniform");
            cfg.duration_us = 30 * kSecond;
            cfg.recv_noise_mode = NoiseMode::Uniform;
            cfg.recv_noise_us = noises[ni];
            SetGaussianJitter(cfg.delay_ab, 5.0, 4.0);
            SetGaussianJitter(cfg.delay_ba, 5.0, 4.0);
            char name[96];
            std::snprintf(name, sizeof(name), "MC-F14_uniform_%uus", noises[ni]);
            cfg.name = name;
            add(cfg);
        }
        {
            ExperimentConfig cfg = BaseConfig("MC-F14_lognormal");
            cfg.duration_us = 30 * kSecond;
            cfg.recv_noise_mode = NoiseMode::LogNormal;
            cfg.recv_noise_us = 200;
            cfg.recv_noise_sigma = 0.7;
            SetGaussianJitter(cfg.delay_ab, 5.0, 4.0);
            SetGaussianJitter(cfg.delay_ba, 5.0, 4.0);
            cfg.name = "MC-F14_lognormal_200us";
            add(cfg);
        }
        return exps;
    }

    if (key == "f15" || key == "mc-f15" || key == "mc_f15" || key == "mcf15") {
        const double rates[] = {0.5, 1, 2, 5};
        const double skews[] = {100, 200, 500};
        for (size_t ri = 0; ri < sizeof(rates)/sizeof(rates[0]); ++ri) {
            for (size_t si = 0; si < sizeof(skews)/sizeof(skews[0]); ++si) {
                for (int sign = -1; sign <= 1; sign += 2) {
                    ExperimentConfig cfg = BaseConfig("MC-F15");
                    cfg.duration_us = 120 * kSecond;
                    cfg.send_rate_hz = rates[ri];
                    cfg.sync_interval_us = 2 * kSecond;
                    SetGaussianJitter(cfg.delay_ab, 5.0, 4.0);
                    SetGaussianJitter(cfg.delay_ba, 5.0, 4.0);
                    cfg.drift_ppm_a = skews[si] * sign;
                    cfg.drift_ppm_b = -cfg.drift_ppm_a;
                    char name[128];
                    std::snprintf(name, sizeof(name), "MC-F15_rate%.1f_skew%+.0f", rates[ri], cfg.drift_ppm_a);
                    cfg.name = name;
                    add(cfg);
                }
            }
        }
        return exps;
    }

    if (key == "f16" || key == "mc-f16" || key == "mc_f16" || key == "mcf16") {
        ExperimentConfig cfg = BaseConfig("MC-F16_control_plane");
        cfg.duration_us = 60 * kSecond;
        SetLossBoth(cfg, 0.01);
        cfg.use_sync_loss = true;
        cfg.loss_sync_ab.loss_rate = 0.30;
        cfg.loss_sync_ba.loss_rate = 0.30;
        cfg.reorder_prob_ab = 0.02;
        cfg.reorder_prob_ba = 0.02;
        add(cfg);
        return exps;
    }

    if (key == "f17" || key == "mc-f17" || key == "mc_f17" || key == "mcf17") {
        const double spikes_s[] = {2, 6, 10};
        for (size_t si = 0; si < sizeof(spikes_s)/sizeof(spikes_s[0]); ++si) {
            ExperimentConfig cfg = BaseConfig("MC-F17");
            cfg.duration_us = 60 * kSecond;
            SetSymmetricBase(cfg, 50.0);
            SetGaussianJitter(cfg.delay_ab, 2.0, 4.0);
            SetGaussianJitter(cfg.delay_ba, 2.0, 4.0);
            cfg.delay_ab.spike_prob = 0.001;
            cfg.delay_ba.spike_prob = 0.001;
            cfg.delay_ab.spike_delay_us = (uint32_t)std::llround(spikes_s[si] * 1000000.0);
            cfg.delay_ba.spike_delay_us = cfg.delay_ab.spike_delay_us;
            char name[96];
            std::snprintf(name, sizeof(name), "MC-F17_spike%.0fs", spikes_s[si]);
            cfg.name = name;
            add(cfg);
        }
        return exps;
    }

    if (key == "f18" || key == "mc-f18" || key == "mc_f18" || key == "mcf18") {
        const double losses[] = {0.50, 0.80};
        for (size_t li = 0; li < sizeof(losses)/sizeof(losses[0]); ++li) {
            ExperimentConfig cfg = BaseConfig("MC-F18");
            cfg.duration_us = 60 * kSecond;
            SetLossBoth(cfg, losses[li]);
            cfg.loss_ab.burst_start_prob = 0.05;
            cfg.loss_ba.burst_start_prob = 0.05;
            cfg.loss_ab.burst_len_min = 20;
            cfg.loss_ab.burst_len_max = 100;
            cfg.loss_ba.burst_len_min = 20;
            cfg.loss_ba.burst_len_max = 100;
            char name[96];
            std::snprintf(name, sizeof(name), "MC-F18_loss%.0f", losses[li] * 100.0);
            cfg.name = name;
            add(cfg);
        }
        return exps;
    }

    if (key == "f19" || key == "mc-f19" || key == "mc_f19" || key == "mcf19") {
        const double skews[] = {2000, 3000, 5000};
        for (size_t si = 0; si < sizeof(skews)/sizeof(skews[0]); ++si) {
            for (int sign = -1; sign <= 1; sign += 2) {
                ExperimentConfig cfg = BaseConfig("MC-F19");
                cfg.duration_us = 60 * kSecond;
                cfg.send_rate_hz = 10.0;
                SetGaussianJitter(cfg.delay_ab, 5.0, 4.0);
                SetGaussianJitter(cfg.delay_ba, 5.0, 4.0);
                cfg.drift_ppm_a = skews[si] * sign;
                cfg.drift_ppm_b = -cfg.drift_ppm_a;
                char name[96];
                std::snprintf(name, sizeof(name), "MC-F19_skew%+.0f", cfg.drift_ppm_a);
                cfg.name = name;
                add(cfg);
            }
        }
        return exps;
    }

    if (key == "f20" || key == "mc-f20" || key == "mc_f20" || key == "mcf20") {
        const int64_t offsets[] = {20000000LL, -20000000LL, 3600000000LL, -3600000000LL};
        for (size_t oi = 0; oi < sizeof(offsets)/sizeof(offsets[0]); ++oi) {
            ExperimentConfig cfg = BaseConfig("MC-F20");
            cfg.duration_us = 30 * kSecond;
            cfg.offset_b_us = offsets[oi];
            char name[96];
            std::snprintf(name, sizeof(name), "MC-F20_offset_%+lldus", (long long)offsets[oi]);
            cfg.name = name;
            add(cfg);
        }
        return exps;
    }

    if (key == "f21" || key == "mc-f21" || key == "mc_f21" || key == "mcf21" ||
        key == "video" || key == "mc-video" || key == "mc_video" || key == "video-link") {
        const double bases_ms[] = {20, 60, 120};
        const double jitters_ms[] = {1.0, 5.0};
        const double sigmas[] = {0.35, 0.60};
        struct Profile {
            const char* tag;
            bool congestion;
            bool burst_loss;
            bool path_step;
        };
        const Profile profiles[] = {
            {"base", false, false, false},
            {"cong", true, false, false},
            {"burst", false, true, false},
            {"step", false, false, true},
        };
        for (size_t bi = 0; bi < sizeof(bases_ms) / sizeof(bases_ms[0]); ++bi) {
            for (size_t ji = 0; ji < sizeof(jitters_ms) / sizeof(jitters_ms[0]); ++ji) {
                for (size_t si = 0; si < sizeof(sigmas) / sizeof(sigmas[0]); ++si) {
                    for (size_t pi = 0; pi < sizeof(profiles) / sizeof(profiles[0]); ++pi) {
                        const Profile& p = profiles[pi];
                        ExperimentConfig cfg = BaseConfig("MC-F21");
                        cfg.duration_us = 60 * kSecond;
                        SetSymmetricBase(cfg, bases_ms[bi]);
                        cfg.delay_ab.jitter_mode = JitterMode::LogNormal;
                        cfg.delay_ba.jitter_mode = JitterMode::LogNormal;
                        cfg.delay_ab.jitter_us = (uint32_t)std::llround(jitters_ms[ji] * 1000.0);
                        cfg.delay_ba.jitter_us = cfg.delay_ab.jitter_us;
                        cfg.delay_ab.lognormal_sigma = sigmas[si];
                        cfg.delay_ba.lognormal_sigma = sigmas[si];

                        if (p.congestion) {
                            const uint32_t amp_us = (bases_ms[bi] <= 60.0) ? 20000 : 60000;
                            cfg.delay_ab.queue_amp_us = amp_us;
                            cfg.delay_ba.queue_amp_us = amp_us;
                            cfg.delay_ab.queue_period_us = 8 * kSecond;
                            cfg.delay_ba.queue_period_us = 8 * kSecond;
                        }
                        if (p.burst_loss) {
                            SetLossBoth(cfg, 0.02);
                            cfg.loss_ab.burst_start_prob = 0.05;
                            cfg.loss_ba.burst_start_prob = 0.05;
                            cfg.loss_ab.burst_len_min = 5;
                            cfg.loss_ab.burst_len_max = 30;
                            cfg.loss_ba.burst_len_min = 5;
                            cfg.loss_ba.burst_len_max = 30;
                        }
                        if (p.path_step) {
                            cfg.delay_ab.step_at_us = 30 * kSecond;
                            cfg.delay_ba.step_at_us = 30 * kSecond;
                            const int32_t step_us = (bases_ms[bi] <= 60.0) ? 20000 : 50000;
                            cfg.delay_ab.step_delta_us = step_us;
                            cfg.delay_ba.step_delta_us = step_us;
                            cfg.delay_ab.step2_at_us = 45 * kSecond;
                            cfg.delay_ba.step2_at_us = 45 * kSecond;
                            cfg.delay_ab.step2_delta_us = -step_us / 2;
                            cfg.delay_ba.step2_delta_us = -step_us / 2;
                        }

                        char name[160];
                        std::snprintf(
                            name, sizeof(name),
                            "MC-F21_video_b%.0f_j%.1f_s%.2f_%s",
                            bases_ms[bi], jitters_ms[ji], sigmas[si], p.tag);
                        cfg.name = name;
                        add(cfg);
                    }
                }
            }
        }
        return exps;
    }

    if (key == "f22" || key == "mc-f22" || key == "mc_f22" || key == "mcf22" ||
        key == "clock-step" || key == "clock_step" || key == "mc-clock-step" || key == "mc_clock_step") {
        struct StepCase {
            const char* tag;
            int64_t step_us;
            uint64_t duration_us;
            uint64_t step_time_us;
        };
        const StepCase cases[] = {
            {"small", 2000000LL, 60 * kSecond, 20 * kSecond},
            {"large", 30000000LL, 120 * kSecond, 40 * kSecond},
        };
        for (size_t ci = 0; ci < sizeof(cases) / sizeof(cases[0]); ++ci) {
            const StepCase& sc = cases[ci];
            const int64_t steps[] = {sc.step_us, -sc.step_us};
            for (int si = 0; si < 2; ++si) {
                const char* dir = (steps[si] >= 0) ? "fwd" : "back";
                for (int side = 0; side < 2; ++side) {
                    ExperimentConfig cfg = BaseConfig("MC-F22");
                    cfg.duration_us = sc.duration_us;
                    SetSymmetricBase(cfg, 60.0);
                    SetGaussianJitter(cfg.delay_ab, 3.0, 4.0);
                    SetGaussianJitter(cfg.delay_ba, 3.0, 4.0);
                    cfg.clock_step_enabled = true;
                    cfg.clock_step_time_us = sc.step_time_us;
                    if (side == 0) {
                        cfg.clock_step_a_us = steps[si];
                    } else {
                        cfg.clock_step_b_us = steps[si];
                    }
                    char name[160];
                    std::snprintf(
                        name, sizeof(name),
                        "MC-F22_step_%s_%s_%s",
                        sc.tag, (side == 0) ? "a" : "b", dir);
                    cfg.name = name;
                    add(cfg);
                }
            }
        }
        return exps;
    }

    return exps;
}

static std::vector<ExperimentConfig> BuildMonteCarlo(uint64_t seed, unsigned count)
{
    std::vector<ExperimentConfig> exps;
    exps.reserve(count);

    PCGRandom rng;
    rng.Seed(seed, 0xA5A5A5A5ULL);

    static const double kRateOptions[] = {10.0, 30.0, 60.0, 120.0, 240.0};
    static const double kSyncOptions[] = {0.5, 2.0, 5.0, 10.0};

    for (unsigned i = 0; i < count; ++i) {
        ExperimentConfig cfg = BaseConfig("mc_0000");

        // Base latencies (ms)
        const double base_ab_ms = RandRangeDouble(rng, 2.0, 200.0);
        const double base_ba_ms = RandRangeDouble(rng, 2.0, 200.0);
        cfg.delay_ab.base_delay_us = (uint32_t)(base_ab_ms * 1000.0);
        cfg.delay_ba.base_delay_us = (uint32_t)(base_ba_ms * 1000.0);

        // Jitter (ms)
        const double jitter_ab_ms = RandRangeDouble(rng, 0.0, 30.0);
        const double jitter_ba_ms = RandRangeDouble(rng, 0.0, 30.0);
        cfg.delay_ab.jitter_us = (uint32_t)(jitter_ab_ms * 1000.0);
        cfg.delay_ba.jitter_us = (uint32_t)(jitter_ba_ms * 1000.0);

        // Loss rate
        const double loss = RandRangeDouble(rng, 0.0, 0.1);
        cfg.loss_ab.loss_rate = loss;
        cfg.loss_ba.loss_rate = loss;

        // Drift (ppm)
        const double drift = RandRangeDouble(rng, 0.0, 2000.0);
        const bool flip = (rng.Next() & 1) != 0;
        cfg.drift_ppm_a = flip ? drift : -drift;
        cfg.drift_ppm_b = -cfg.drift_ppm_a;

        // Send rate and sync interval
        cfg.send_rate_hz = kRateOptions[rng.Next() % (sizeof(kRateOptions) / sizeof(kRateOptions[0]))];
        cfg.sync_interval_us = (uint64_t)(kSyncOptions[rng.Next() % (sizeof(kSyncOptions) / sizeof(kSyncOptions[0]))] * 1000000.0);

        // Spikes
        cfg.delay_ab.spike_prob = RandRangeDouble(rng, 0.0, 0.05);
        cfg.delay_ba.spike_prob = cfg.delay_ab.spike_prob;
        const double spike_ms = RandRangeDouble(rng, 0.0, 120.0);
        cfg.delay_ab.spike_delay_us = (uint32_t)(spike_ms * 1000.0);
        cfg.delay_ba.spike_delay_us = cfg.delay_ab.spike_delay_us;

        // Bimodal
        cfg.delay_ab.bimodal_prob = RandRangeDouble(rng, 0.0, 0.3);
        cfg.delay_ba.bimodal_prob = cfg.delay_ab.bimodal_prob;
        const double bimodal_ms = RandRangeDouble(rng, 0.0, 120.0);
        cfg.delay_ab.bimodal_delay_us = (uint32_t)(bimodal_ms * 1000.0);
        cfg.delay_ba.bimodal_delay_us = cfg.delay_ab.bimodal_delay_us;

        // Duration (shorter to allow many samples)
        cfg.duration_us = 30 * 1000 * 1000ULL;

        char name[32];
        std::snprintf(name, sizeof(name), "mc_%04u", i + 1);
        cfg.name = name;

        exps.push_back(cfg);
    }

    return exps;
}

static std::vector<ExperimentConfig> BuildMonteCarloAxis(
    uint64_t seed,
    unsigned count,
    const string& axis,
    double drift_min,
    double drift_max,
    double axis_min,
    double axis_max)
{
    std::vector<ExperimentConfig> exps;
    exps.reserve(count);

    PCGRandom rng;
    rng.Seed(seed, 0x5A5A5A5AULL);

    const double base_latency_ms = 50.0;
    const double jitter_baseline_ms = 5.0;
    const double loss_baseline = 0.0;
    const double sync_baseline_s = 2.0;
    const double rate_baseline_hz = 60.0;

    for (unsigned i = 0; i < count; ++i) {
        ExperimentConfig cfg = BaseConfig("mc_axis_0000");

        const double drift = RandRangeDouble(rng, drift_min, drift_max);
        const bool flip = (rng.Next() & 1) != 0;
        cfg.drift_ppm_a = flip ? drift : -drift;
        cfg.drift_ppm_b = -cfg.drift_ppm_a;

        // Baselines
        cfg.delay_ab.base_delay_us = (uint32_t)(base_latency_ms * 1000.0);
        cfg.delay_ba.base_delay_us = (uint32_t)(base_latency_ms * 1000.0);
        cfg.delay_ab.jitter_us = (uint32_t)(jitter_baseline_ms * 1000.0);
        cfg.delay_ba.jitter_us = (uint32_t)(jitter_baseline_ms * 1000.0);
        cfg.loss_ab.loss_rate = loss_baseline;
        cfg.loss_ba.loss_rate = loss_baseline;
        cfg.sync_interval_us = (uint64_t)(sync_baseline_s * 1000000.0);
        cfg.send_rate_hz = rate_baseline_hz;

        const double axis_value = RandRangeDouble(rng, axis_min, axis_max);

        if (axis == "jitter_ms") {
            cfg.delay_ab.jitter_us = (uint32_t)(axis_value * 1000.0);
            cfg.delay_ba.jitter_us = (uint32_t)(axis_value * 1000.0);
        }
        else if (axis == "loss") {
            cfg.loss_ab.loss_rate = axis_value;
            cfg.loss_ba.loss_rate = axis_value;
        }
        else if (axis == "asymmetry_ms") {
            const double half = axis_value * 0.5;
            double ab = base_latency_ms - half;
            double ba = base_latency_ms + half;
            if (ab < 2.0) {
                ab = 2.0;
                ba = 2.0 + axis_value;
            }
            cfg.delay_ab.base_delay_us = (uint32_t)(ab * 1000.0);
            cfg.delay_ba.base_delay_us = (uint32_t)(ba * 1000.0);
        }
        else if (axis == "sync_s") {
            cfg.sync_interval_us = (uint64_t)(axis_value * 1000000.0);
        }
        else if (axis == "rate_hz") {
            cfg.send_rate_hz = axis_value;
        }
        else if (axis == "latency_ms") {
            cfg.delay_ab.base_delay_us = (uint32_t)(axis_value * 1000.0);
            cfg.delay_ba.base_delay_us = (uint32_t)(axis_value * 1000.0);
        }

        cfg.duration_us = 30 * 1000 * 1000ULL;

        char name[48];
        std::snprintf(name, sizeof(name), "mc_%s_%04u", axis.c_str(), i + 1);
        cfg.name = name;

        exps.push_back(cfg);
    }

    return exps;
}

//------------------------------------------------------------------------------
// Auto-scaling + progress helpers

static unsigned GetHardwareThreads()
{
    unsigned threads = std::thread::hardware_concurrency();
    if (threads == 0) {
        threads = 1;
    }
    return threads;
}

static string FormatDuration(double seconds)
{
    if (seconds < 0.0) {
        seconds = 0.0;
    }
    const int total = (int)std::llround(seconds);
    const int h = total / 3600;
    const int m = (total / 60) % 60;
    const int s = total % 60;
    char buf[32];
    if (h > 0) {
        std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
    } else {
        std::snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    }
    return string(buf);
}

static string RenderBar(double fraction, int width)
{
    if (fraction < 0.0) fraction = 0.0;
    if (fraction > 1.0) fraction = 1.0;
    const int filled = (int)std::llround(fraction * width);
    string bar;
    bar.reserve((size_t)width);
    for (int i = 0; i < width; ++i) {
        bar.push_back(i < filled ? '#' : '-');
    }
    return bar;
}

static uint64_t Hash64(const string& s);

static double CalibrateThroughput(
    const std::vector<ExperimentConfig>& experiments,
    uint64_t base_seed,
    unsigned threads,
    double duration_seconds)
{
    if (experiments.empty() || duration_seconds <= 0.0) {
        return 0.0;
    }

    std::atomic<size_t> completed(0);
    std::atomic<size_t> next_index(0);
    std::atomic<bool> stop(false);

    const auto start = std::chrono::steady_clock::now();
    const auto end_time = start + std::chrono::duration<double>(duration_seconds);

    auto worker = [&]() {
        for (;;) {
            if (stop.load(std::memory_order_relaxed)) {
                break;
            }
            const auto now = std::chrono::steady_clock::now();
            if (now >= end_time) {
                stop.store(true, std::memory_order_relaxed);
                break;
            }
            const size_t idx = next_index.fetch_add(1, std::memory_order_relaxed);
            const ExperimentConfig& cfg = experiments[idx % experiments.size()];
            const uint64_t seed = base_seed ^ Hash64(cfg.name) ^ (uint64_t)idx;
            RunExperiment(cfg, seed, nullptr);
            completed.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::vector<std::thread> workers;
    workers.reserve(threads);
    for (unsigned i = 0; i < threads; ++i) {
        workers.emplace_back(worker);
    }
    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].join();
    }

    const auto finish = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = finish - start;
    const double seconds = elapsed.count();
    if (seconds <= 0.0) {
        return 0.0;
    }
    return (double)completed.load(std::memory_order_relaxed) / seconds;
}

static bool StartsWith(const string& s, const char* prefix)
{
    return s.rfind(prefix, 0) == 0;
}

static double MaxP95Time(const ExperimentMetrics& m)
{
    return std::max(m.ab.time_error_us.Percentile(0.95), m.ba.time_error_us.Percentile(0.95));
}

static double MaxP99Time(const ExperimentMetrics& m)
{
    return std::max(m.ab.time_error_us.Percentile(0.99), m.ba.time_error_us.Percentile(0.99));
}

static double MaxP95Owd(const ExperimentMetrics& m)
{
    return std::max(m.ab.owd_error_us.Percentile(0.95), m.ba.owd_error_us.Percentile(0.95));
}

static double MaxP99Owd(const ExperimentMetrics& m)
{
    return std::max(m.ab.owd_error_us.Percentile(0.99), m.ba.owd_error_us.Percentile(0.99));
}

static bool ValidateExperiment(const ExperimentConfig& cfg, const ExperimentMetrics& m, string& reason)
{
    const string& name = cfg.name;
    const bool synced = m.sync_a && m.sync_b;

    const bool expect_unsynced =
        StartsWith(name, "UT-B01") ||
        StartsWith(name, "UT-B02") ||
        StartsWith(name, "UT-C01") ||
        StartsWith(name, "UT-D11");

    const bool expect_synced =
        StartsWith(name, "UT-B04") ||
        StartsWith(name, "UT-B05") ||
        StartsWith(name, "UT-B06") ||
        StartsWith(name, "UT-B08") ||
        StartsWith(name, "UT-B09") ||
        StartsWith(name, "UT-B10") ||
        StartsWith(name, "UT-C02") ||
        StartsWith(name, "UT-C03") ||
        StartsWith(name, "UT-C04") ||
        StartsWith(name, "UT-C05") ||
        StartsWith(name, "UT-C06") ||
        StartsWith(name, "UT-C07") ||
        StartsWith(name, "UT-C08") ||
        StartsWith(name, "UT-C09") ||
        StartsWith(name, "UT-C10") ||
        StartsWith(name, "UT-D01") ||
        StartsWith(name, "UT-D02") ||
        StartsWith(name, "UT-D04") ||
        StartsWith(name, "UT-D05") ||
        StartsWith(name, "UT-D06") ||
        StartsWith(name, "UT-D07") ||
        StartsWith(name, "UT-D08") ||
        StartsWith(name, "UT-D09") ||
        StartsWith(name, "UT-D10") ||
        StartsWith(name, "UT-D12") ||
        StartsWith(name, "UT-E");

    if (StartsWith(name, "UT-B03")) {
        return true;
    }

    if (expect_unsynced && synced) {
        reason = "expected unsynchronized but both peers synced";
        return false;
    }
    if (expect_synced && !synced) {
        reason = "expected synchronized but did not sync";
        return false;
    }

    if (!synced) {
        return true;
    }

    const double time_p95 = MaxP95Time(m);
    const double time_p99 = MaxP99Time(m);
    const double owd_p95 = MaxP95Owd(m);
    const double owd_p99 = MaxP99Owd(m);

    const size_t time_samples = m.ab.time_error_us.Count() + m.ba.time_error_us.Count();
    const size_t owd_samples = m.ab.owd_error_us.Count() + m.ba.owd_error_us.Count();
    if (time_samples == 0) {
        reason = "no time-error samples while synchronized";
        return false;
    }
    if (owd_samples == 0) {
        reason = "no owd-error samples while synchronized";
        return false;
    }

    if (StartsWith(name, "UT-D03") ||
        StartsWith(name, "UT-B09") ||
        StartsWith(name, "UT-E14")) {
        return true;
    }

    if (StartsWith(name, "UT-E15_clock_step_large") ||
        StartsWith(name, "UT-E16_clock_step_large")) {
        return true;
    }

    if (StartsWith(name, "UT-B04") || StartsWith(name, "UT-C02")) {
        if (time_p95 > 5000.0 || owd_p95 > 5000.0) {
            reason = "tight-bound exceeded (p95 > 5ms)";
            return false;
        }
    }

    if (StartsWith(name, "UT-C03")) {
        const uint32_t min_ab = (m.ab.min_true_owd_us == UINT32_MAX) ? 0 : m.ab.min_true_owd_us;
        const uint32_t min_ba = (m.ba.min_true_owd_us == UINT32_MAX) ? 0 : m.ba.min_true_owd_us;
        const uint32_t min_avg = (min_ab + min_ba) / 2;
        const uint32_t est_avg = (m.min_owd_est_a_us + m.min_owd_est_b_us) / 2;
        const uint32_t diff = (est_avg > min_avg) ? (est_avg - min_avg) : (min_avg - est_avg);
        if (diff > 5000) {
            reason = "min OWD estimate deviates >5ms from true min";
            return false;
        }
    }

    if (StartsWith(name, "UT-C04")) {
        const uint32_t base_us = cfg.delay_ab.base_delay_us;
        if (m.min_owd_est_a_us > base_us + 5000 || m.min_owd_est_b_us > base_us + 5000) {
            reason = "min OWD estimate too high for spike scenario";
            return false;
        }
    }

    if (StartsWith(name, "UT-C08")) {
        if (time_p95 > 100000.0) {
            reason = "time p95 exceeds 100ms under asymmetry";
            return false;
        }
    }

    if (StartsWith(name, "UT-E03") ||
        StartsWith(name, "UT-E01") ||
        StartsWith(name, "UT-E02") ||
        StartsWith(name, "UT-E14") ||
        StartsWith(name, "UT-B06") ||
        StartsWith(name, "UT-B09")) {
        if (time_p99 > 20000000.0 || owd_p99 > 20000000.0) {
            reason = "errors too high (p99 > 20s)";
            return false;
        }
    }
    else if (StartsWith(name, "UT-E") || StartsWith(name, "UT-D") || StartsWith(name, "UT-C") || StartsWith(name, "UT-B")) {
        if (time_p99 > 5000000.0 || owd_p99 > 5000000.0) {
            reason = "errors too high (p99 > 5s)";
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// CLI helpers

struct CliOptions
{
    bool list_only = false;
    string suite;
    string csv_path = "experiments.csv";
    std::vector<string> only;
    string match;
    uint64_t seed = 0xC0FFEEULL;
    unsigned threads = 0;
    unsigned monte_carlo = 0;
    string mc_family;
    unsigned mc_seeds = 10;
    string mc_axis;
    bool auto_run = false;
    double target_seconds = 600.0;
    double calibrate_seconds = 10.0;
    bool progress = true;
    bool assert_results = false;
    double mc_drift_min = 0.0;
    double mc_drift_max = 2000.0;
    double mc_axis_min = 0.0;
    double mc_axis_max = 0.0;
    bool mc_axis_min_set = false;
    bool mc_axis_max_set = false;
    bool mc_drift_min_set = false;
    bool mc_drift_max_set = false;
    double poll_rate_hz = 0.0;
    bool poll_rate_hz_set = false;
    bool batch_mode = false;
};

static bool ShouldRun(const ExperimentConfig& cfg, const CliOptions& opt)
{
    if (!opt.only.empty()) {
        for (size_t i = 0; i < opt.only.size(); ++i) {
            if (cfg.name == opt.only[i]) {
                return true;
            }
        }
        return false;
    }
    if (!opt.match.empty()) {
        return cfg.name.find(opt.match) != string::npos;
    }
    return true;
}

static CliOptions ParseArgs(int argc, char** argv)
{
    CliOptions opt;
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (std::strcmp(arg, "--list") == 0) {
            opt.list_only = true;
        }
        else if (std::strcmp(arg, "--suite") == 0 && i + 1 < argc) {
            opt.suite = argv[++i];
        }
        else if (std::strcmp(arg, "--csv") == 0 && i + 1 < argc) {
            opt.csv_path = argv[++i];
        }
        else if (std::strcmp(arg, "--only") == 0 && i + 1 < argc) {
            opt.only.push_back(argv[++i]);
        }
        else if (std::strcmp(arg, "--match") == 0 && i + 1 < argc) {
            opt.match = argv[++i];
        }
        else if (std::strcmp(arg, "--seed") == 0 && i + 1 < argc) {
            opt.seed = (uint64_t)std::strtoull(argv[++i], nullptr, 10);
        }
        else if (std::strcmp(arg, "--threads") == 0 && i + 1 < argc) {
            opt.threads = (unsigned)std::strtoul(argv[++i], nullptr, 10);
        }
        else if (std::strcmp(arg, "--poll-rate-hz") == 0 && i + 1 < argc) {
            opt.poll_rate_hz = std::strtod(argv[++i], nullptr);
            opt.poll_rate_hz_set = true;
        }
        else if (std::strcmp(arg, "--montecarlo") == 0 && i + 1 < argc) {
            opt.monte_carlo = (unsigned)std::strtoul(argv[++i], nullptr, 10);
        }
        else if (std::strcmp(arg, "--mc-family") == 0 && i + 1 < argc) {
            opt.mc_family = argv[++i];
        }
        else if (std::strcmp(arg, "--mc-seeds") == 0 && i + 1 < argc) {
            opt.mc_seeds = (unsigned)std::strtoul(argv[++i], nullptr, 10);
            if (opt.mc_seeds == 0) {
                opt.mc_seeds = 1;
            }
        }
        else if (std::strcmp(arg, "--auto") == 0) {
            opt.auto_run = true;
        }
        else if (std::strcmp(arg, "--batch") == 0 || std::strcmp(arg, "--no-wallclock") == 0) {
            opt.batch_mode = true;
        }
        else if (std::strcmp(arg, "--target-minutes") == 0 && i + 1 < argc) {
            opt.target_seconds = std::strtod(argv[++i], nullptr) * 60.0;
        }
        else if (std::strcmp(arg, "--target-seconds") == 0 && i + 1 < argc) {
            opt.target_seconds = std::strtod(argv[++i], nullptr);
        }
        else if (std::strcmp(arg, "--calibrate-seconds") == 0 && i + 1 < argc) {
            opt.calibrate_seconds = std::strtod(argv[++i], nullptr);
        }
        else if (std::strcmp(arg, "--no-progress") == 0) {
            opt.progress = false;
        }
        else if (std::strcmp(arg, "--progress") == 0) {
            opt.progress = true;
        }
        else if (std::strcmp(arg, "--assert") == 0) {
            opt.assert_results = true;
        }
        else if (std::strcmp(arg, "--mc-axis") == 0 && i + 1 < argc) {
            opt.mc_axis = argv[++i];
        }
        else if (std::strcmp(arg, "--mc-drift-min") == 0 && i + 1 < argc) {
            opt.mc_drift_min = std::strtod(argv[++i], nullptr);
            opt.mc_drift_min_set = true;
        }
        else if (std::strcmp(arg, "--mc-drift-max") == 0 && i + 1 < argc) {
            opt.mc_drift_max = std::strtod(argv[++i], nullptr);
            opt.mc_drift_max_set = true;
        }
        else if (std::strcmp(arg, "--mc-axis-min") == 0 && i + 1 < argc) {
            opt.mc_axis_min = std::strtod(argv[++i], nullptr);
            opt.mc_axis_min_set = true;
        }
        else if (std::strcmp(arg, "--mc-axis-max") == 0 && i + 1 < argc) {
            opt.mc_axis_max = std::strtod(argv[++i], nullptr);
            opt.mc_axis_max_set = true;
        }
        else if (std::strcmp(arg, "--help") == 0 || std::strcmp(arg, "-h") == 0) {
            std::cout << "Usage: experiments [--list] [--suite name] [--csv path] [--only name] [--match substring] [--seed n] [--threads n] [--poll-rate-hz v] [--auto] [--batch|--no-wallclock] [--target-minutes n] [--target-seconds n] [--calibrate-seconds n] [--no-progress] [--assert] [--montecarlo n] [--mc-family name] [--mc-seeds n] [--mc-axis name] [--mc-drift-min v] [--mc-drift-max v] [--mc-axis-min v] [--mc-axis-max v]\n";
            std::exit(0);
        }
    }
    return opt;
}

static void WriteCsvHeader(std::ofstream& out)
{
    out << "name,duration_s,send_rate_hz,sync_interval_s,sync_interval_ab_s,sync_interval_ba_s,base_ab_ms,base_ba_ms,jitter_ab_ms,jitter_ba_ms,";
    out << "loss_ab,loss_ba,burst_start,burst_len_min,burst_len_max,spike_prob,spike_delay_ms,bimodal_prob,bimodal_delay_ms,";
    out << "queue_amp_ms,queue_period_s,ramp_ms_per_s,drift_a_ppm,drift_b_ppm,offset_a_s,offset_b_s,";
    out << "sent_ab,recv_ab,lost_ab,sent_ba,recv_ba,lost_ba,sync_time_a_s,sync_time_b_s,metrics_start_s,";
    out << "time_err_mean_ab_us,time_err_p95_ab_us,time_err_p99_ab_us,time_err_max_ab_us,time_err_count_ab,";
    out << "time_err_mean_ba_us,time_err_p95_ba_us,time_err_p99_ba_us,time_err_max_ba_us,time_err_count_ba,";
    out << "poll_time_err_mean_ab_us,poll_time_err_p95_ab_us,poll_time_err_p99_ab_us,poll_time_err_max_ab_us,poll_time_err_count_ab,";
    out << "poll_time_err_mean_ba_us,poll_time_err_p95_ba_us,poll_time_err_p99_ba_us,poll_time_err_max_ba_us,poll_time_err_count_ba,";
    out << "owd_err_mean_ab_us,owd_err_p95_ab_us,owd_err_p99_ab_us,owd_err_max_ab_us,owd_err_count_ab,";
    out << "owd_err_mean_ba_us,owd_err_p95_ba_us,owd_err_p99_ba_us,owd_err_max_ba_us,owd_err_count_ba,";
    out << "min_owd_est_a_us,min_owd_est_b_us,min_owd_true_avg_us";
    out << "\n";
}

static void WriteCsvRow(std::ofstream& out, const ExperimentConfig& cfg, const ExperimentMetrics& m)
{
    const double duration_s = (double)cfg.duration_us / 1000000.0;
    const double sync_interval_s = (double)cfg.sync_interval_us / 1000000.0;
    const uint64_t sync_ab_us = (cfg.sync_interval_ab_us > 0) ? cfg.sync_interval_ab_us : cfg.sync_interval_us;
    const uint64_t sync_ba_us = (cfg.sync_interval_ba_us > 0) ? cfg.sync_interval_ba_us : cfg.sync_interval_us;
    const double sync_ab_s = (double)sync_ab_us / 1000000.0;
    const double sync_ba_s = (double)sync_ba_us / 1000000.0;

    const double time_mean_ab = m.ab.time_error_us.Mean();
    const double time_p95_ab = m.ab.time_error_us.Percentile(0.95);
    const double time_p99_ab = m.ab.time_error_us.Percentile(0.99);
    const double time_max_ab = m.ab.time_error_us.Max();

    const double time_mean_ba = m.ba.time_error_us.Mean();
    const double time_p95_ba = m.ba.time_error_us.Percentile(0.95);
    const double time_p99_ba = m.ba.time_error_us.Percentile(0.99);
    const double time_max_ba = m.ba.time_error_us.Max();

    const double owd_mean_ab = m.ab.owd_error_us.Mean();
    const double owd_p95_ab = m.ab.owd_error_us.Percentile(0.95);
    const double owd_p99_ab = m.ab.owd_error_us.Percentile(0.99);
    const double owd_max_ab = m.ab.owd_error_us.Max();

    const double owd_mean_ba = m.ba.owd_error_us.Mean();
    const double owd_p95_ba = m.ba.owd_error_us.Percentile(0.95);
    const double owd_p99_ba = m.ba.owd_error_us.Percentile(0.99);
    const double owd_max_ba = m.ba.owd_error_us.Max();

    uint32_t min_ab = (m.ab.min_true_owd_us == UINT32_MAX) ? 0 : m.ab.min_true_owd_us;
    uint32_t min_ba = (m.ba.min_true_owd_us == UINT32_MAX) ? 0 : m.ba.min_true_owd_us;
    uint32_t min_avg = (min_ab + min_ba) / 2;

    out << cfg.name << ",";
    out << duration_s << ",";
    out << cfg.send_rate_hz << ",";
    out << sync_interval_s << ",";
    out << sync_ab_s << ",";
    out << sync_ba_s << ",";
    out << cfg.delay_ab.base_delay_us / 1000.0 << ",";
    out << cfg.delay_ba.base_delay_us / 1000.0 << ",";
    out << cfg.delay_ab.jitter_us / 1000.0 << ",";
    out << cfg.delay_ba.jitter_us / 1000.0 << ",";
    out << cfg.loss_ab.loss_rate << ",";
    out << cfg.loss_ba.loss_rate << ",";
    out << cfg.loss_ab.burst_start_prob << ",";
    out << cfg.loss_ab.burst_len_min << ",";
    out << cfg.loss_ab.burst_len_max << ",";
    out << cfg.delay_ab.spike_prob << ",";
    out << cfg.delay_ab.spike_delay_us / 1000.0 << ",";
    out << cfg.delay_ab.bimodal_prob << ",";
    out << cfg.delay_ab.bimodal_delay_us / 1000.0 << ",";
    out << cfg.delay_ab.queue_amp_us / 1000.0 << ",";
    out << cfg.delay_ab.queue_period_us / 1000000.0 << ",";
    out << cfg.delay_ab.ramp_us_per_s / 1000.0 << ",";
    out << cfg.drift_ppm_a << ",";
    out << cfg.drift_ppm_b << ",";
    out << cfg.offset_a_us / 1000000.0 << ",";
    out << cfg.offset_b_us / 1000000.0 << ",";

    out << m.ab.sent << ",";
    out << m.ab.received << ",";
    out << m.ab.lost << ",";
    out << m.ba.sent << ",";
    out << m.ba.received << ",";
    out << m.ba.lost << ",";
    out << (m.sync_a ? (double)m.sync_time_a_us / 1000000.0 : 0.0) << ",";
    out << (m.sync_b ? (double)m.sync_time_b_us / 1000000.0 : 0.0) << ",";
    out << (m.metrics_start_set ? (double)m.metrics_start_us / 1000000.0 : 0.0) << ",";

    out << time_mean_ab << ",";
    out << time_p95_ab << ",";
    out << time_p99_ab << ",";
    out << time_max_ab << ",";
    out << m.ab.time_error_us.Count() << ",";

    out << time_mean_ba << ",";
    out << time_p95_ba << ",";
    out << time_p99_ba << ",";
    out << time_max_ba << ",";
    out << m.ba.time_error_us.Count() << ",";

    out << m.ab.poll_time_error_us.Mean() << ",";
    out << m.ab.poll_time_error_us.Percentile(0.95) << ",";
    out << m.ab.poll_time_error_us.Percentile(0.99) << ",";
    out << m.ab.poll_time_error_us.Max() << ",";
    out << m.ab.poll_time_error_us.Count() << ",";

    out << m.ba.poll_time_error_us.Mean() << ",";
    out << m.ba.poll_time_error_us.Percentile(0.95) << ",";
    out << m.ba.poll_time_error_us.Percentile(0.99) << ",";
    out << m.ba.poll_time_error_us.Max() << ",";
    out << m.ba.poll_time_error_us.Count() << ",";

    out << owd_mean_ab << ",";
    out << owd_p95_ab << ",";
    out << owd_p99_ab << ",";
    out << owd_max_ab << ",";
    out << m.ab.owd_error_us.Count() << ",";

    out << owd_mean_ba << ",";
    out << owd_p95_ba << ",";
    out << owd_p99_ba << ",";
    out << owd_max_ba << ",";
    out << m.ba.owd_error_us.Count() << ",";

    out << m.min_owd_est_a_us << ",";
    out << m.min_owd_est_b_us << ",";
    out << min_avg;
    out << "\n";
}

static void PrintSummary(const ExperimentConfig& cfg, const ExperimentMetrics& m)
{
    const double poll_p95_ab = m.ab.poll_time_error_us.Percentile(0.95);
    const double poll_p95_ba = m.ba.poll_time_error_us.Percentile(0.95);
    const double owd_p95_ab = m.ab.owd_error_us.Percentile(0.95);
    const double owd_p95_ba = m.ba.owd_error_us.Percentile(0.95);

    std::cout << cfg.name << ": "
              << "syncA=" << (m.sync_a ? "yes" : "no")
              << " syncB=" << (m.sync_b ? "yes" : "no")
              << " poll_p95_ab_us=" << poll_p95_ab
              << " poll_p95_ba_us=" << poll_p95_ba
              << " owd_p95_ab_us=" << owd_p95_ab
              << " owd_p95_ba_us=" << owd_p95_ba
              << " samples_ab=" << m.ab.poll_time_error_us.Count()
              << " samples_ba=" << m.ba.poll_time_error_us.Count()
              << "\n";
}

static uint64_t Hash64(const string& s)
{
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < s.size(); ++i) {
        h ^= (uint64_t)(unsigned char)s[i];
        h *= 1099511628211ULL;
    }
    return h;
}

static std::vector<ExperimentConfig> BuildExperimentsFromOptions(const CliOptions& opt)
{
    std::vector<ExperimentConfig> experiments;
    if (!opt.suite.empty()) {
        const string suite = ToLower(opt.suite);
        if (suite == "testplan") {
            experiments = BuildTestPlan();
        }
        else if (suite == "default" || suite == "experiments") {
            experiments = BuildExperiments();
        }
        else if (suite == "mc-family" || suite == "mcfamily") {
            const string fam = opt.mc_family.empty() ? string("all") : opt.mc_family;
            experiments = BuildMonteCarloFamily(fam, opt.mc_seeds);
        }
    }
    else if (!opt.mc_family.empty()) {
        experiments = BuildMonteCarloFamily(opt.mc_family, opt.mc_seeds);
    }
    else if (opt.monte_carlo > 0) {
        if (!opt.mc_axis.empty()) {
            double axis_min = opt.mc_axis_min_set ? opt.mc_axis_min : 0.0;
            double axis_max = opt.mc_axis_max_set ? opt.mc_axis_max : 0.0;

            if (!opt.mc_axis_min_set || !opt.mc_axis_max_set) {
                if (opt.mc_axis == "jitter_ms") {
                    axis_min = 0.0; axis_max = 30.0;
                } else if (opt.mc_axis == "loss") {
                    axis_min = 0.0; axis_max = 0.1;
                } else if (opt.mc_axis == "asymmetry_ms") {
                    axis_min = 0.0; axis_max = 150.0;
                } else if (opt.mc_axis == "sync_s") {
                    axis_min = 0.5; axis_max = 10.0;
                } else if (opt.mc_axis == "rate_hz") {
                    axis_min = 10.0; axis_max = 240.0;
                } else if (opt.mc_axis == "latency_ms") {
                    axis_min = 2.0; axis_max = 200.0;
                }
            }

            const double drift_min = opt.mc_drift_min_set ? opt.mc_drift_min : 0.0;
            const double drift_max = opt.mc_drift_max_set ? opt.mc_drift_max : 2000.0;

            experiments = BuildMonteCarloAxis(
                opt.seed,
                opt.monte_carlo,
                opt.mc_axis,
                drift_min,
                drift_max,
                axis_min,
                axis_max);
        }
        else {
            experiments = BuildMonteCarlo(opt.seed, opt.monte_carlo);
        }
    }
    else {
        experiments = BuildExperiments();
    }

    if (opt.poll_rate_hz_set) {
        for (size_t i = 0; i < experiments.size(); ++i) {
            experiments[i].poll_rate_hz = opt.poll_rate_hz;
        }
    }
    return experiments;
}

int main(int argc, char** argv)
{
    CliOptions opt = ParseArgs(argc, argv);

    if (opt.batch_mode) {
        opt.auto_run = false;
        opt.progress = false;
    }
    if (opt.poll_rate_hz_set && opt.poll_rate_hz < 0.0) {
        opt.poll_rate_hz = 0.0;
    }

    if (opt.threads == 0) {
        opt.threads = GetHardwareThreads();
    }

    std::vector<ExperimentConfig> experiments = BuildExperimentsFromOptions(opt);

    if (experiments.empty()) {
        std::cerr << "No experiments defined for the selected options.\n";
        return 1;
    }

    if (opt.list_only) {
        for (size_t i = 0; i < experiments.size(); ++i) {
            std::cout << experiments[i].name << "\n";
        }
        return 0;
    }

    std::ofstream csv(opt.csv_path.c_str());
    if (!csv) {
        std::cerr << "Failed to open CSV output: " << opt.csv_path << "\n";
        return 1;
    }

    std::vector<size_t> base_ids;
    base_ids.reserve(experiments.size());
    for (size_t i = 0; i < experiments.size(); ++i) {
        if (ShouldRun(experiments[i], opt)) {
            base_ids.push_back(i);
        }
    }
    if (base_ids.empty()) {
        std::cerr << "No experiments selected.\n";
        return 1;
    }

    size_t repeat_count = 1;
    size_t extra = 0;

    if (opt.auto_run) {
        std::cout << "Auto-scaling enabled: calibrating for " << opt.calibrate_seconds
                  << "s on " << opt.threads << " threads...\n";
        std::vector<ExperimentConfig> calibration;
        calibration.reserve(base_ids.size());
        for (size_t i = 0; i < base_ids.size(); ++i) {
            calibration.push_back(experiments[base_ids[i]]);
        }
        const double throughput = CalibrateThroughput(
            calibration,
            opt.seed,
            opt.threads,
            opt.calibrate_seconds);
        const double safe_throughput = (throughput > 0.0) ? throughput : 1.0;
        const double est_seconds = (double)base_ids.size() / safe_throughput;
        const double target = opt.target_seconds;
        size_t desired_runs = (size_t)std::llround(safe_throughput * target);
        if (desired_runs < 1) {
            desired_runs = 1;
        }

        const bool using_random_mc = opt.suite.empty() && opt.monte_carlo > 0 && opt.mc_family.empty();

        if (using_random_mc) {
            if (desired_runs < base_ids.size()) {
                desired_runs = base_ids.size();
                std::cout << "Baseline Monte Carlo count exceeds target; keeping full coverage.\n";
            }
            CliOptions auto_opt = opt;
            auto_opt.monte_carlo = (unsigned)desired_runs;
            experiments = BuildExperimentsFromOptions(auto_opt);
            base_ids.clear();
            for (size_t i = 0; i < experiments.size(); ++i) {
                if (ShouldRun(experiments[i], opt)) {
                    base_ids.push_back(i);
                }
            }
            repeat_count = 1;
            extra = 0;
            std::cout << "Monte Carlo auto target: " << base_ids.size()
                      << " samples (~" << FormatDuration(target) << ")\n";
        }
        else if (desired_runs <= base_ids.size()) {
            std::cout << "Baseline suite estimated at ~" << FormatDuration(est_seconds)
                      << "; keeping full coverage without reduction.\n";
        }
        else {
            repeat_count = desired_runs / base_ids.size();
            extra = desired_runs % base_ids.size();
            if (repeat_count < 1) {
                repeat_count = 1;
            }
            std::cout << "Baseline suite estimated at ~" << FormatDuration(est_seconds)
                      << "; repeating coverage " << repeat_count << "x to target ~"
                      << FormatDuration(target) << ".\n";
        }
    }

    const size_t selected_count = base_ids.size();
    if (selected_count == 0) {
        std::cerr << "No experiments selected after auto-scaling.\n";
        return 1;
    }

    struct RunItem {
        size_t exp_index;
        size_t base_index;
        uint64_t seed_offset;
    };

    std::vector<RunItem> run_items;
    run_items.reserve(selected_count * repeat_count + extra);

    const bool unique_seeds = (repeat_count > 1 || extra > 0);
    for (size_t r = 0; r < repeat_count; ++r) {
        for (size_t i = 0; i < selected_count; ++i) {
            RunItem item;
            item.exp_index = base_ids[i];
            item.base_index = i;
            item.seed_offset = unique_seeds ? (uint64_t)run_items.size() : 0;
            run_items.push_back(item);
        }
    }
    for (size_t i = 0; i < extra && i < selected_count; ++i) {
        RunItem item;
        item.exp_index = base_ids[i];
        item.base_index = i;
        item.seed_offset = unique_seeds ? (uint64_t)run_items.size() : 0;
        run_items.push_back(item);
    }

    const size_t run_count = run_items.size();
    if (run_count == 0) {
        std::cerr << "No experiments selected.\n";
        return 1;
    }

    if (opt.threads > run_count) {
        opt.threads = (unsigned)run_count;
    }

    std::vector<std::atomic<bool>> base_done(selected_count);
    for (size_t i = 0; i < base_done.size(); ++i) {
        base_done[i].store(false, std::memory_order_relaxed);
    }

    struct ProgressState {
        std::atomic<size_t> completed;
        std::atomic<size_t> covered;
        std::atomic<size_t> current_run;
        std::atomic<uint32_t> current_permille;
    } progress_state;

    progress_state.completed.store(0, std::memory_order_relaxed);
    progress_state.covered.store(0, std::memory_order_relaxed);
    progress_state.current_run.store((size_t)-1, std::memory_order_relaxed);
    progress_state.current_permille.store(0, std::memory_order_relaxed);

    std::vector<ExperimentMetrics> results(run_count);
    std::atomic<size_t> next_index(0);
    std::vector<string> failure_reasons(run_count);

    auto worker = [&](unsigned worker_id) {
        for (;;) {
            const size_t idx = next_index.fetch_add(1);
            if (idx >= run_count) {
                break;
            }
            const RunItem& item = run_items[idx];
            const ExperimentConfig& cfg = experiments[item.exp_index];
            const uint64_t seed = opt.seed ^ Hash64(cfg.name) ^ item.seed_offset;
            std::atomic<uint32_t>* progress_ptr = nullptr;
            if (worker_id == 0 && opt.progress) {
                progress_state.current_run.store(idx, std::memory_order_relaxed);
                progress_state.current_permille.store(0, std::memory_order_relaxed);
                progress_ptr = &progress_state.current_permille;
            }
            results[idx] = RunExperiment(cfg, seed, progress_ptr);
            progress_state.completed.fetch_add(1, std::memory_order_relaxed);

            const size_t base_index = item.base_index;
            if (base_index < base_done.size()) {
                const bool was_done = base_done[base_index].exchange(true, std::memory_order_relaxed);
                if (!was_done) {
                    progress_state.covered.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }
    };

    std::thread progress_thread;
    if (opt.progress) {
        progress_thread = std::thread([&]() {
            const auto start = std::chrono::steady_clock::now();
            size_t last_len = 0;
            for (;;) {
                const size_t done = progress_state.completed.load(std::memory_order_relaxed);
                const size_t covered = progress_state.covered.load(std::memory_order_relaxed);
                const size_t current = progress_state.current_run.load(std::memory_order_relaxed);
                const uint32_t permille = progress_state.current_permille.load(std::memory_order_relaxed);
                const auto now = std::chrono::steady_clock::now();
                const std::chrono::duration<double> elapsed = now - start;

                const double overall_frac = (run_count > 0) ? ((double)done / (double)run_count) : 1.0;
                const double coverage_frac = (selected_count > 0) ? ((double)covered / (double)selected_count) : 1.0;
                const double current_frac = (double)permille / 1000.0;

                double eta = 0.0;
                if (done > 0) {
                    eta = elapsed.count() * ((double)(run_count - done) / (double)done);
                }

                string current_name;
                if (current < run_count) {
                    current_name = experiments[run_items[current].exp_index].name;
                }

                string line;
                line.reserve(256);
                line += "[";
                line += RenderBar(overall_frac, 20);
                line += "] ";
                line += std::to_string((int)std::llround(overall_frac * 100.0));
                line += "% ";
                line += "(" + std::to_string(done) + "/" + std::to_string(run_count) + ") ";
                line += "cov ";
                line += std::to_string((int)std::llround(coverage_frac * 100.0));
                line += "% ";
                line += "(" + std::to_string(covered) + "/" + std::to_string(selected_count) + ") ";
                line += "ETA ";
                line += FormatDuration(eta);
                line += " | [";
                line += RenderBar(current_frac, 16);
                line += "] ";
                line += std::to_string((int)std::llround(current_frac * 100.0));
                line += "% ";
                if (!current_name.empty()) {
                    line += current_name;
                }

                if (line.size() < last_len) {
                    line.append(last_len - line.size(), ' ');
                }
                last_len = line.size();

                std::cout << "\r" << line << std::flush;

                if (done >= run_count) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            std::cout << "\n";
        });
    }

    std::vector<std::thread> threads;
    threads.reserve(opt.threads);
    for (unsigned i = 0; i < opt.threads; ++i) {
        threads.emplace_back(worker, i);
    }
    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
    if (progress_thread.joinable()) {
        progress_thread.join();
    }

    bool any_failures = false;
    std::vector<string> base_failure_reason(selected_count);
    if (opt.assert_results) {
        for (size_t i = 0; i < run_count; ++i) {
            const ExperimentConfig& cfg = experiments[run_items[i].exp_index];
            string reason;
            if (StartsWith(cfg.name, "UT-")) {
                if (!ValidateExperiment(cfg, results[i], reason)) {
                    any_failures = true;
                    failure_reasons[i] = reason;
                    const size_t base_index = run_items[i].base_index;
                    if (base_index < base_failure_reason.size() && base_failure_reason[base_index].empty()) {
                        base_failure_reason[base_index] = reason;
                    }
                }
            }
        }
    }

    WriteCsvHeader(csv);
    for (size_t i = 0; i < run_count; ++i) {
        const ExperimentConfig& cfg = experiments[run_items[i].exp_index];
        WriteCsvRow(csv, cfg, results[i]);
        PrintSummary(cfg, results[i]);
        if (opt.assert_results) {
            const size_t base_index = run_items[i].base_index;
            if (base_index < base_failure_reason.size() && !base_failure_reason[base_index].empty()) {
                std::cerr << "ASSERT FAIL: " << cfg.name << " -> " << base_failure_reason[base_index] << "\n";
                base_failure_reason[base_index].clear();
            }
        }
    }

    std::cout << "Ran " << run_count << " experiments. CSV: " << opt.csv_path << "\n";

    if (any_failures) {
        std::cerr << "One or more assertions failed.\n";
        return 2;
    }

    return 0;
}
