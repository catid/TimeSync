#include <TimeSync/TimeSync.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using std::string;

//------------------------------------------------------------------------------
// PRNG (PCG)

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
        return (double)Next() / (double)UINT32_MAX;
    }

    uint64_t State = 0;
    uint64_t Inc = 0;
};

static double RandNormal(PCGRandom& rng)
{
    double u1 = rng.NextDouble01();
    double u2 = rng.NextDouble01();
    if (u1 < 1e-12) {
        u1 = 1e-12;
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
                } else {
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
    void Add(double v)
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

        p95.Add(v);
        p99.Add(v);
    }

    size_t Count() const { return count; }

    double Mean() const
    {
        if (count == 0) {
            return 0.0;
        }
        return sum / (double)count;
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

    double Min() const { return (count == 0) ? 0.0 : min; }
    double Max() const { return (count == 0) ? 0.0 : max; }

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
    double sum = 0.0;
    double min = 1e30;
    double max = 0.0;
    mutable bool sorted = false;
    mutable std::vector<double> exact_samples;
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
    double bimodal_high_prob = 0.2;
    uint32_t bimodal_delay_us = 0;
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
    double delay_drop_prob = 0.0;

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

    double current_ppm = 0.0;
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

static double CurrentDriftPpm(ClockModel& model, uint64_t now_us, PCGRandom& rng)
{
    UpdateDriftWander(model, now_us, rng);
    double ppm = model.drift_ppm_base;
    if (model.drift_sine_amp_ppm != 0.0 && model.drift_sine_period_us > 0) {
        const double phase = 2.0 * 3.14159265358979323846 *
            (double)(now_us % model.drift_sine_period_us) / (double)model.drift_sine_period_us;
        ppm += model.drift_sine_amp_ppm * std::sin(phase);
    }
    ppm += model.drift_rw_ppm;
    if (model.drift_step_enabled && now_us >= model.drift_step_time_us) {
        ppm += model.drift_step_delta_ppm;
    }
    model.current_ppm = ppm;
    return ppm;
}

static uint64_t ComputeLocalTimeUsec(uint64_t true_us, ClockModel& model, PCGRandom& rng, bool add_recv_noise)
{
    const double ppm = CurrentDriftPpm(model, true_us, rng);
    const double scale = 1.0 + ppm * 1e-6;
    double local = (double)model.offset_us + (double)true_us * scale;

    if (model.clock_step_enabled && true_us >= model.clock_step_time_us) {
        local += (double)model.clock_step_us;
    }

    if (model.quantize_us > 0) {
        local = std::floor(local / (double)model.quantize_us + 0.5) * (double)model.quantize_us;
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

static double TrueSkewPpm(const ClockModel& local, const ClockModel& remote)
{
    const double scale_local = 1.0 + local.current_ppm * 1e-6;
    const double scale_remote = 1.0 + remote.current_ppm * 1e-6;
    return (scale_remote / scale_local - 1.0) * 1e6;
}

//------------------------------------------------------------------------------
// Method + estimator

enum class MethodKind
{
    Cristian,
    Ntp,
    Ptp,
    TimeSync,
    Piggyback
};

enum class EstimatorKind
{
    Min,
    Quantile,
    Median,
    Regression
};

enum class DisciplineKind
{
    None,
    PLL,
    FLL,
    Hybrid,
    Kalman
};

static const char* MethodName(MethodKind k)
{
    switch (k) {
        case MethodKind::Cristian: return "M1_Cristian";
        case MethodKind::Ntp: return "M2_NTP";
        case MethodKind::Ptp: return "M3_PTP";
        case MethodKind::TimeSync: return "M4_TimeSync";
        case MethodKind::Piggyback: return "M4_Piggyback";
    }
    return "Unknown";
}

static const char* EstimatorName(EstimatorKind k)
{
    switch (k) {
        case EstimatorKind::Min: return "min";
        case EstimatorKind::Quantile: return "pquant";
        case EstimatorKind::Median: return "median";
        case EstimatorKind::Regression: return "regress";
    }
    return "unknown";
}

static const char* DisciplineName(DisciplineKind k)
{
    switch (k) {
        case DisciplineKind::None: return "none";
        case DisciplineKind::PLL: return "pll";
        case DisciplineKind::FLL: return "fll";
        case DisciplineKind::Hybrid: return "hybrid";
        case DisciplineKind::Kalman: return "kalman";
    }
    return "unknown";
}

struct OffsetSample
{
    uint64_t t_us = 0;
    double offset_us = 0.0; // remote - local
};

struct OffsetEstimator
{
    EstimatorKind estimator = EstimatorKind::Median;
    DisciplineKind discipline = DisciplineKind::Hybrid;
    double quantile = 0.01;
    uint64_t window_us = 2 * 1000 * 1000ULL;
    bool initialized = false;

    double offset_est = 0.0;
    double skew_est_ppm = 0.0;

    std::deque<OffsetSample> samples;

    void AddSample(uint64_t t_us, double offset_us)
    {
        OffsetSample s;
        s.t_us = t_us;
        s.offset_us = offset_us;
        samples.push_back(s);
        const uint64_t cutoff = (t_us > window_us) ? (t_us - window_us) : 0;
        while (!samples.empty() && samples.front().t_us < cutoff) {
            samples.pop_front();
        }
        UpdateEstimate(t_us);
    }

    void UpdateEstimate(uint64_t t_us)
    {
        if (samples.empty()) {
            return;
        }
        double raw_offset = samples.back().offset_us;
        double raw_skew_ppm = 0.0;

        if (estimator == EstimatorKind::Min) {
            raw_offset = samples.front().offset_us;
            for (size_t i = 1; i < samples.size(); ++i) {
                raw_offset = std::min(raw_offset, samples[i].offset_us);
            }
        }
        else if (estimator == EstimatorKind::Quantile || estimator == EstimatorKind::Median) {
            std::vector<double> values;
            values.reserve(samples.size());
            for (size_t i = 0; i < samples.size(); ++i) {
                values.push_back(samples[i].offset_us);
            }
            double q = (estimator == EstimatorKind::Median) ? 0.5 : quantile;
            size_t idx = (size_t)std::floor(q * (values.size() - 1));
            std::nth_element(values.begin(), values.begin() + idx, values.end());
            raw_offset = values[idx];
        }
        else if (estimator == EstimatorKind::Regression) {
            // Linear regression on (t, offset)
            double sum_t = 0.0;
            double sum_o = 0.0;
            double sum_tt = 0.0;
            double sum_to = 0.0;
            const double t0 = (double)samples.front().t_us;
            const double denom_scale = 1e-6;
            for (size_t i = 0; i < samples.size(); ++i) {
                const double t = ((double)samples[i].t_us - t0) * denom_scale;
                const double o = samples[i].offset_us;
                sum_t += t;
                sum_o += o;
                sum_tt += t * t;
                sum_to += t * o;
            }
            const double n = (double)samples.size();
            const double denom = (n * sum_tt - sum_t * sum_t);
            double slope = 0.0;
            if (std::fabs(denom) > 1e-9) {
                slope = (n * sum_to - sum_t * sum_o) / denom; // us per second
            }
            raw_skew_ppm = slope;
            const double t_curr = ((double)t_us - t0) * denom_scale;
            const double intercept = (sum_o - slope * sum_t) / n;
            raw_offset = intercept + slope * t_curr;
        }

        if (!initialized) {
            offset_est = raw_offset;
            skew_est_ppm = raw_skew_ppm;
            initialized = true;
            return;
        }

        const double dt_s = samples.size() > 1
            ? (double)(samples.back().t_us - samples.front().t_us) / 1000000.0
            : 0.0;

        switch (discipline) {
            case DisciplineKind::None:
                offset_est = raw_offset;
                if (estimator == EstimatorKind::Regression) {
                    skew_est_ppm = raw_skew_ppm;
                }
                break;
            case DisciplineKind::PLL: {
                const double alpha = 0.2;
                offset_est = offset_est + alpha * (raw_offset - offset_est);
                break;
            }
            case DisciplineKind::FLL: {
                if (samples.size() >= 2 && dt_s > 1e-6) {
                    const double diff = raw_offset - offset_est;
                    const double beta = 0.2;
                    skew_est_ppm = skew_est_ppm + beta * (diff / dt_s);
                    offset_est = raw_offset;
                }
                break;
            }
            case DisciplineKind::Hybrid: {
                const double alpha = 0.2;
                const double beta = 0.1;
                offset_est = offset_est + alpha * (raw_offset - offset_est);
                if (samples.size() >= 2 && dt_s > 1e-6) {
                    skew_est_ppm = skew_est_ppm + beta * ((raw_offset - offset_est) / dt_s);
                }
                break;
            }
            case DisciplineKind::Kalman: {
                // Alpha-beta filter (Kalman-lite)
                const double dt = std::max(dt_s, 1e-3);
                const double alpha = 0.85;
                const double beta = 0.005;
                // predict
                offset_est = offset_est + skew_est_ppm * dt;
                const double resid = raw_offset - offset_est;
                offset_est = offset_est + alpha * resid;
                skew_est_ppm = skew_est_ppm + (beta / dt) * resid;
                break;
            }
        }
    }

    bool Ready() const { return initialized; }
};

struct WindowedEnvelope
{
    EstimatorKind estimator = EstimatorKind::Min;
    double quantile = 0.01;
    uint64_t window_us = 2 * 1000 * 1000ULL;
    std::deque<OffsetSample> samples;

    void Add(uint64_t t_us, double delta_us)
    {
        OffsetSample s;
        s.t_us = t_us;
        s.offset_us = delta_us;
        samples.push_back(s);
        const uint64_t cutoff = (t_us > window_us) ? (t_us - window_us) : 0;
        while (!samples.empty() && samples.front().t_us < cutoff) {
            samples.pop_front();
        }
    }

    bool Ready() const { return !samples.empty(); }

    double Value() const
    {
        if (samples.empty()) {
            return 0.0;
        }
        if (estimator == EstimatorKind::Min) {
            double v = samples.front().offset_us;
            for (size_t i = 1; i < samples.size(); ++i) {
                v = std::min(v, samples[i].offset_us);
            }
            return v;
        }
        double q = (estimator == EstimatorKind::Median) ? 0.5 : quantile;
        std::vector<double> values;
        values.reserve(samples.size());
        for (size_t i = 0; i < samples.size(); ++i) {
            values.push_back(samples[i].offset_us);
        }
        size_t idx = (size_t)std::floor(q * (values.size() - 1));
        std::nth_element(values.begin(), values.begin() + idx, values.end());
        return values[idx];
    }
};

//------------------------------------------------------------------------------
// Benchmark structures

enum class ScenarioKind
{
    Standard,
    Teleop
};

struct ScenarioConfig
{
    string name;
    ScenarioKind kind = ScenarioKind::Standard;
    bool train = true;
    uint64_t duration_us = 30 * 1000 * 1000ULL;
    double send_rate_hz = 60.0;
    double probe_rate_hz = 10.0;
    uint64_t mindelta_interval_us = 1000000;
    uint64_t metrics_warmup_us = 1000 * 1000ULL;

    DelayModel delay_ab;
    DelayModel delay_ba;
    LossModel loss_ab;
    LossModel loss_ba;
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
    double drift_step_delta_ppm_a = 0.0;
    double drift_step_delta_ppm_b = 0.0;

    int64_t offset_a_us = 0;
    int64_t offset_b_us = 0;
    uint32_t quantize_us = 0;
    uint32_t recv_noise_us = 0;
    NoiseMode recv_noise_mode = NoiseMode::Uniform;
    double recv_noise_sigma = 0.5;
    bool clock_step_enabled = false;
    uint64_t clock_step_time_us = 0;
    int64_t clock_step_a_us = 0;
    int64_t clock_step_b_us = 0;

    double overhead_budget_bps = 0.0; // if >0, adjust probe/sample rate
    uint32_t payload_bytes = 64;
};

struct MethodConfig
{
    MethodKind kind = MethodKind::TimeSync;
    EstimatorKind estimator = EstimatorKind::Min;
    DisciplineKind discipline = DisciplineKind::Hybrid;
    double quantile = 0.01;
    uint64_t window_us = 2 * 1000 * 1000ULL;
    int sample_stride = 1; // stamp every N packets
    double probe_rate_hz = 10.0;
    uint64_t mindelta_interval_us = 1000000;
    bool timestamp_data = true;
    bool use_timesync = true;
    bool use_envelope = true;
    string variant_name;
};

struct RunItem
{
    ScenarioConfig scenario;
    MethodConfig method;
    uint64_t seed = 0;
};

struct DirectionMetrics
{
    uint64_t sent = 0;
    uint64_t received = 0;
    uint64_t lost = 0;
    uint64_t deadline_tp = 0;
    uint64_t deadline_fp = 0;
    uint64_t deadline_fn = 0;
    uint64_t deadline_tn = 0;

    SampleStats offset_err_us;
    SampleStats skew_err_ppm;
    SampleStats owd_err_us;

    bool converged = false;
    uint64_t converge_time_us = 0;
};

struct BenchmarkMetrics
{
    DirectionMetrics ab;
    DirectionMetrics ba;

    double overhead_bps = 0.0;
    double cpu_ops = 0.0;
    double mem_bytes = 0.0;

    bool synced_a = false;
    bool synced_b = false;
    uint64_t sync_time_a_us = 0;
    uint64_t sync_time_b_us = 0;

    // Teleop metrics
    double teleop_rms_error = 0.0;
    double teleop_max_error = 0.0;
    double teleop_settle_s = 0.0;
};

//------------------------------------------------------------------------------
// Message simulation

enum class MsgKind
{
    Data,
    ProbeReq,
    ProbeResp,
    MinDelta,
    TeleopCmd,
    TeleopState
};

struct Message
{
    bool from_a = true;
    MsgKind kind = MsgKind::Data;
    bool stamped = true;

    uint64_t send_true_us = 0;
    uint64_t send_local_us = 0;
    uint64_t true_remote_local_at_send = 0;

    Counter24 ts24 = 0;
    Counter23 app_ts23 = 0;
    Counter24 min_delta = 0;
    uint32_t env_delta_us = 0;

    // Probe fields
    uint64_t t1_local = 0;
    uint64_t t2_remote = 0;
    uint64_t t3_remote = 0;
};

struct ArrivalEvent
{
    uint64_t deliver_true_us = 0;
    Message msg;
    uint32_t true_delay_us = 0;
};

struct ArrivalCompare
{
    bool operator()(const ArrivalEvent& a, const ArrivalEvent& b) const
    {
        return a.deliver_true_us > b.deliver_true_us;
    }
};

struct NodeState
{
    ClockModel clock;
    OffsetEstimator estimator;
    WindowedEnvelope local_env;
    double peer_env_value = 0.0;
    bool peer_env_valid = false;
    TimeSynchronizer timesync;

    uint64_t last_min_delta_send_us = 0;
    uint64_t last_probe_send_us = 0;
    uint64_t last_data_send_us = 0;

    bool synced = false;
    uint64_t sync_time_us = 0;
};

//------------------------------------------------------------------------------
// Helpers

static uint64_t Hash64(const string& s)
{
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < s.size(); ++i) {
        h ^= (uint64_t)(uint8_t)s[i];
        h *= 1099511628211ULL;
    }
    return h;
}

static void MaybeSetSyncTime(bool ready, uint64_t now_us, bool& flag, uint64_t& out)
{
    if (!flag && ready) {
        flag = true;
        out = now_us;
    }
}

static double AbsDouble(double v)
{
    return (v < 0.0) ? -v : v;
}

//------------------------------------------------------------------------------
// Core simulation (standard scenarios)

static void ApplyBudget(MethodConfig& method, const ScenarioConfig& scenario)
{
    if (scenario.overhead_budget_bps <= 0.0) {
        return;
    }
    const double budget = scenario.overhead_budget_bps;
    const double stamp_bytes = method.timestamp_data ? 3.0 : 0.0;
    const double min_delta_bytes = (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) ? 3.0 : 0.0;
    const double probe_bytes = (method.kind == MethodKind::Cristian) ? 16.0 : 24.0;

    const double base_stamp = 2.0 * (scenario.send_rate_hz / std::max(1, method.sample_stride)) * stamp_bytes;
    const double base_min_delta = 2.0 * (1.0e6 / std::max<uint64_t>(method.mindelta_interval_us, 1ULL)) * min_delta_bytes;

    if (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) {
        double remaining = budget - base_min_delta;
        if (remaining < 0.0) {
            remaining = 0.0;
        }
        if (stamp_bytes > 0.0) {
            double max_stamp_rate = remaining / (2.0 * stamp_bytes);
            if (max_stamp_rate < scenario.send_rate_hz) {
                int stride = (int)std::ceil(scenario.send_rate_hz / std::max(max_stamp_rate, 1.0));
                if (stride < 1) stride = 1;
                method.sample_stride = stride;
            }
        }
        return;
    }

    // Probe-based methods: adjust probe rate
    double remaining = budget - base_stamp;
    if (remaining < 0.0) {
        remaining = 0.0;
    }
    const double max_probe_rate = remaining / (2.0 * probe_bytes);
    method.probe_rate_hz = std::max(0.0, max_probe_rate);
}

static double EstimateOverheadBps(const MethodConfig& method, const ScenarioConfig& scenario)
{
    const double stamp_bytes = method.timestamp_data ? 3.0 : 0.0;
    const double min_delta_bytes = (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) ? 3.0 : 0.0;
    const double probe_bytes = (method.kind == MethodKind::Cristian) ? 16.0 : 24.0;

    double total = 0.0;
    total += 2.0 * (scenario.send_rate_hz / std::max(1, method.sample_stride)) * stamp_bytes;
    if (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) {
        total += 2.0 * (1.0e6 / std::max<uint64_t>(method.mindelta_interval_us, 1ULL)) * min_delta_bytes;
    } else {
        total += 2.0 * method.probe_rate_hz * probe_bytes;
    }
    return total;
}

static void AddOffsetError(DirectionMetrics& dm, double err_us, uint64_t now_us)
{
    dm.offset_err_us.Add(AbsDouble(err_us));

    const double threshold = 1000.0; // 1ms
    if (!dm.converged) {
        if (std::fabs(err_us) <= threshold) {
            dm.converged = true;
            dm.converge_time_us = now_us;
        }
    }
}

static void AddSkewError(DirectionMetrics& dm, double err_ppm)
{
    dm.skew_err_ppm.Add(AbsDouble(err_ppm));
}

static void AddOwdError(DirectionMetrics& dm, double err_us)
{
    dm.owd_err_us.Add(AbsDouble(err_us));
}

static void UpdateDeadlineMetrics(DirectionMetrics& dm, double owd_true_us, double owd_est_us, double deadline_us)
{
    const bool true_late = owd_true_us > deadline_us;
    const bool est_late = owd_est_us > deadline_us;
    if (true_late && est_late) dm.deadline_tp++;
    else if (!true_late && est_late) dm.deadline_fp++;
    else if (true_late && !est_late) dm.deadline_fn++;
    else dm.deadline_tn++;
}

static void ScheduleWithEffects(
    std::priority_queue<ArrivalEvent, std::vector<ArrivalEvent>, ArrivalCompare>& arrivals,
    const Message& msg,
    uint64_t now_us,
    uint32_t delay_us,
    double reorder_prob,
    uint32_t reorder_advance_us,
    uint32_t reorder_delay_us,
    double duplicate_prob,
    uint32_t duplicate_delay_us,
    PCGRandom& rng)
{
    ArrivalEvent ev;
    ev.deliver_true_us = now_us + delay_us;
    ev.msg = msg;
    ev.true_delay_us = delay_us;

    if (reorder_prob > 0.0 && rng.NextDouble01() < reorder_prob) {
        if (reorder_delay_us > 0) {
            ev.true_delay_us += reorder_delay_us;
            ev.deliver_true_us = now_us + ev.true_delay_us;
        } else {
            const uint32_t advance = (reorder_advance_us > 0) ? reorder_advance_us : (ev.true_delay_us / 2);
            if (ev.true_delay_us > advance) {
                ev.true_delay_us -= advance;
                ev.deliver_true_us = now_us + ev.true_delay_us;
            }
        }
    }

    arrivals.push(ev);

    if (duplicate_prob > 0.0 && rng.NextDouble01() < duplicate_prob) {
        ArrivalEvent dup = ev;
        dup.deliver_true_us += duplicate_delay_us;
        dup.true_delay_us += duplicate_delay_us;
        arrivals.push(dup);
    }
}

static BenchmarkMetrics RunScenario(const ScenarioConfig& scenario_in, const MethodConfig& method_in, uint64_t seed)
{
    ScenarioConfig scenario = scenario_in;
    MethodConfig method = method_in;

    ApplyBudget(method, scenario);

    PCGRandom rng;
    rng.Seed(seed, 0xBADC0FFEULL);

    BenchmarkMetrics metrics;
    metrics.overhead_bps = EstimateOverheadBps(method, scenario);

    NodeState node_a;
    NodeState node_b;

    node_a.clock.drift_ppm_base = scenario.drift_ppm_a;
    node_a.clock.drift_sine_amp_ppm = scenario.drift_sine_amp_ppm;
    node_a.clock.drift_sine_period_us = scenario.drift_sine_period_us;
    node_a.clock.drift_rw_step_ppm = scenario.drift_rw_step_ppm;
    node_a.clock.drift_rw_step_interval_us = scenario.drift_rw_step_interval_us;
    node_a.clock.drift_step_enabled = scenario.drift_step_enabled;
    node_a.clock.drift_step_time_us = scenario.drift_step_time_us;
    node_a.clock.drift_step_delta_ppm = scenario.drift_step_delta_ppm_a;
    node_a.clock.offset_us = scenario.offset_a_us;
    node_a.clock.quantize_us = scenario.quantize_us;
    node_a.clock.recv_noise_us = scenario.recv_noise_us;
    node_a.clock.recv_noise_mode = scenario.recv_noise_mode;
    node_a.clock.recv_noise_sigma = scenario.recv_noise_sigma;
    node_a.clock.clock_step_enabled = scenario.clock_step_enabled;
    node_a.clock.clock_step_time_us = scenario.clock_step_time_us;
    node_a.clock.clock_step_us = scenario.clock_step_a_us;

    node_b.clock = node_a.clock;
    node_b.clock.drift_ppm_base = scenario.drift_ppm_b;
    node_b.clock.drift_step_delta_ppm = scenario.drift_step_delta_ppm_b;
    node_b.clock.offset_us = scenario.offset_b_us;
    node_b.clock.clock_step_us = scenario.clock_step_b_us;

    node_a.estimator.estimator = method.estimator;
    node_a.estimator.discipline = method.discipline;
    node_a.estimator.quantile = method.quantile;
    node_a.estimator.window_us = method.window_us;
    node_b.estimator = node_a.estimator;

    node_a.local_env.estimator = method.estimator;
    node_a.local_env.quantile = method.quantile;
    node_a.local_env.window_us = method.window_us;
    node_b.local_env = node_a.local_env;

    DelayModel delay_ab = scenario.delay_ab;
    DelayModel delay_ba = scenario.delay_ba;
    LossModel loss_ab = scenario.loss_ab;
    LossModel loss_ba = scenario.loss_ba;

    const uint64_t data_interval_us = (scenario.send_rate_hz > 0.0)
        ? (uint64_t)std::llround(1000000.0 / scenario.send_rate_hz)
        : 0;
    const uint64_t probe_interval_us = (method.probe_rate_hz > 0.0)
        ? (uint64_t)std::llround(1000000.0 / method.probe_rate_hz)
        : 0;

    uint64_t next_data_ab = data_interval_us ? 0 : UINT64_MAX;
    uint64_t next_data_ba = data_interval_us ? 0 : UINT64_MAX;
    uint64_t next_probe_ab = probe_interval_us ? 0 : UINT64_MAX;
    uint64_t next_probe_ba = probe_interval_us ? 0 : UINT64_MAX;
    uint64_t next_mindelta_ab = method.mindelta_interval_us ? method.mindelta_interval_us : UINT64_MAX;
    uint64_t next_mindelta_ba = method.mindelta_interval_us ? method.mindelta_interval_us : UINT64_MAX;

    uint64_t now_us = 0;
    uint64_t data_seq_ab = 0;
    uint64_t data_seq_ba = 0;
    bool late_ab_injected = false;
    bool late_ba_injected = false;

    std::priority_queue<ArrivalEvent, std::vector<ArrivalEvent>, ArrivalCompare> arrivals;

    uint64_t metrics_start_us = scenario.metrics_warmup_us;

    const double deadline_us = 50000.0; // 50ms default

    while (now_us <= scenario.duration_us) {
        uint64_t next_arrival = arrivals.empty() ? UINT64_MAX : arrivals.top().deliver_true_us;
        uint64_t next_time = next_arrival;
        if (next_data_ab < next_time) next_time = next_data_ab;
        if (next_data_ba < next_time) next_time = next_data_ba;
        if (next_probe_ab < next_time) next_time = next_probe_ab;
        if (next_probe_ba < next_time) next_time = next_probe_ba;
        if (next_mindelta_ab < next_time) next_time = next_mindelta_ab;
        if (next_mindelta_ba < next_time) next_time = next_mindelta_ba;

        if (next_time == UINT64_MAX) {
            break;
        }
        now_us = next_time;

        while (!arrivals.empty() && arrivals.top().deliver_true_us == now_us) {
            ArrivalEvent ev = arrivals.top();
            arrivals.pop();

            const bool to_b = ev.msg.from_a;
            NodeState& recv_node = to_b ? node_b : node_a;
            NodeState& send_node = to_b ? node_a : node_b;
            DirectionMetrics& dm = to_b ? metrics.ab : metrics.ba;
            DelayModel& dmodel = to_b ? delay_ab : delay_ba;

            const uint64_t local_recv = ComputeLocalTimeUsec(now_us, recv_node.clock, rng, true);
            const uint64_t true_remote_local = ev.msg.true_remote_local_at_send;
            const double true_offset = (double)ev.msg.send_local_us - (double)true_remote_local; // remote - local
            const double true_skew_ppm = TrueSkewPpm(recv_node.clock, send_node.clock);

            dm.received++;

            if (ev.msg.kind == MsgKind::Data || ev.msg.kind == MsgKind::TeleopState || ev.msg.kind == MsgKind::TeleopCmd) {
                if (method.kind == MethodKind::TimeSync) {
                    if (!ev.msg.stamped) {
                        continue;
                    }
                    const unsigned owd_est = recv_node.timesync.OnAuthenticatedDatagramTimestamp(ev.msg.ts24, local_recv);
                    if (recv_node.timesync.IsSynchronized()) {
                        MaybeSetSyncTime(true, now_us, recv_node.synced, recv_node.sync_time_us);
                    }
                    if (now_us >= metrics_start_us && owd_est > 0) {
                        const double owd_err = (double)owd_est - (double)ev.true_delay_us;
                        AddOwdError(dm, owd_err);
                        UpdateDeadlineMetrics(dm, ev.true_delay_us, (double)owd_est, deadline_us);
                    }
                    if (now_us >= metrics_start_us && recv_node.timesync.IsSynchronized()) {
                        if (ev.msg.app_ts23 != 0) {
                            const uint64_t est_local = recv_node.timesync.FromLocalTime23(local_recv, ev.msg.app_ts23);
                            const double err = (double)est_local - (double)true_remote_local;
                            AddOffsetError(dm, err, now_us);
                        }
                    }
                } else if (method.kind == MethodKind::Piggyback) {
                    if (!ev.msg.stamped) {
                        continue;
                    }
                    // Windowed envelope using packet delta
                    const double delta_us = (double)local_recv - (double)ev.msg.send_local_us;
                    recv_node.local_env.Add(now_us, delta_us);
                    if (recv_node.local_env.Ready() && recv_node.peer_env_valid) {
                        const double offset_est = (recv_node.peer_env_value - recv_node.local_env.Value()) * 0.5;
                        recv_node.estimator.AddSample(now_us, offset_est);
                        MaybeSetSyncTime(recv_node.estimator.Ready(), now_us, recv_node.synced, recv_node.sync_time_us);
                    }
                    if (now_us >= metrics_start_us && recv_node.estimator.Ready()) {
                        const double offset_est = recv_node.estimator.offset_est;
                        const double owd_est = (double)local_recv - (double)ev.msg.send_local_us - offset_est;
                        AddOwdError(dm, owd_est - (double)ev.true_delay_us);
                        UpdateDeadlineMetrics(dm, ev.true_delay_us, owd_est, deadline_us);
                        AddOffsetError(dm, offset_est - true_offset, now_us);
                        AddSkewError(dm, recv_node.estimator.skew_est_ppm - true_skew_ppm);
                    }
                } else {
                    // Probe-based methods use estimator offset to compute OWD
                    if (recv_node.estimator.Ready()) {
                        const double offset_est = recv_node.estimator.offset_est;
                        const double owd_est = (double)local_recv - (double)ev.msg.send_local_us - offset_est;
                        if (now_us >= metrics_start_us) {
                            AddOwdError(dm, owd_est - (double)ev.true_delay_us);
                            UpdateDeadlineMetrics(dm, ev.true_delay_us, owd_est, deadline_us);
                            AddOffsetError(dm, offset_est - true_offset, now_us);
                            AddSkewError(dm, recv_node.estimator.skew_est_ppm - true_skew_ppm);
                        }
                    }
                }
            } else if (ev.msg.kind == MsgKind::ProbeReq) {
                // Respond with timestamps
                const uint64_t local_send = ComputeLocalTimeUsec(now_us, recv_node.clock, rng, false);
                Message resp;
                resp.from_a = !ev.msg.from_a;
                resp.kind = MsgKind::ProbeResp;
                resp.send_true_us = now_us;
                resp.send_local_us = local_send;
                resp.t1_local = ev.msg.t1_local;
                resp.t2_remote = local_recv;
                resp.t3_remote = local_send;

                DelayModel& resp_delay_model = to_b ? delay_ba : delay_ab;
                LossModel& resp_loss_model = to_b ? loss_ba : loss_ab;
                const uint32_t delay = resp_delay_model.SampleDelay(now_us, rng);
                if (!resp_loss_model.ShouldDrop(rng, delay)) {
                    ScheduleWithEffects(arrivals, resp, now_us, delay,
                        to_b ? scenario.reorder_prob_ba : scenario.reorder_prob_ab,
                        scenario.reorder_advance_us, scenario.reorder_delay_us,
                        to_b ? scenario.duplicate_prob_ba : scenario.duplicate_prob_ab,
                        scenario.duplicate_delay_us, rng);
                }
            } else if (ev.msg.kind == MsgKind::ProbeResp) {
                // Compute offset sample
                const uint64_t t1 = ev.msg.t1_local;
                const uint64_t t2 = ev.msg.t2_remote;
                const uint64_t t3 = ev.msg.t3_remote;
                const uint64_t t4 = local_recv;

                double offset_sample = 0.0;
                if (method.kind == MethodKind::Cristian) {
                    offset_sample = (double)t2 - ((double)t1 + (double)t4) * 0.5;
                } else {
                    offset_sample = ((double)(t2 - t1) + (double)(t3 - t4)) * 0.5;
                }
                recv_node.estimator.AddSample(now_us, offset_sample);
                MaybeSetSyncTime(recv_node.estimator.Ready(), now_us, recv_node.synced, recv_node.sync_time_us);
            } else if (ev.msg.kind == MsgKind::MinDelta) {
                if (method.kind == MethodKind::TimeSync) {
                    recv_node.timesync.OnPeerMinDeltaTS24(ev.msg.min_delta);
                    if (recv_node.timesync.IsSynchronized()) {
                        MaybeSetSyncTime(true, now_us, recv_node.synced, recv_node.sync_time_us);
                    }
                } else if (method.kind == MethodKind::Piggyback) {
                    recv_node.peer_env_value = (double)ev.msg.env_delta_us;
                    recv_node.peer_env_valid = true;
                }
            }
        }

        // Inject late packet
        if (scenario.inject_late_ab && !late_ab_injected && now_us >= scenario.inject_late_at_us) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            Message msg;
            msg.from_a = true;
            msg.kind = MsgKind::Data;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.ts24 = node_a.timesync.LocalTimeToDatagramTS24(local_send);
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            msg.app_ts23 = node_a.timesync.ToRemoteTime23(local_send);

            ArrivalEvent ev;
            ev.deliver_true_us = now_us + scenario.inject_late_delay_us;
            ev.msg = msg;
            ev.true_delay_us = scenario.inject_late_delay_us;
            arrivals.push(ev);
            late_ab_injected = true;
        }
        if (scenario.inject_late_ba && !late_ba_injected && now_us >= scenario.inject_late_at_us) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            Message msg;
            msg.from_a = false;
            msg.kind = MsgKind::Data;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.ts24 = node_b.timesync.LocalTimeToDatagramTS24(local_send);
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            msg.app_ts23 = node_b.timesync.ToRemoteTime23(local_send);

            ArrivalEvent ev;
            ev.deliver_true_us = now_us + scenario.inject_late_delay_us;
            ev.msg = msg;
            ev.true_delay_us = scenario.inject_late_delay_us;
            arrivals.push(ev);
            late_ba_injected = true;
        }

        // Send data A->B
        if (next_data_ab == now_us) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            Message msg;
            msg.from_a = true;
            msg.kind = MsgKind::Data;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            msg.stamped = true;
            if ((method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) && method.sample_stride > 1) {
                msg.stamped = ((data_seq_ab % (uint64_t)method.sample_stride) == 0);
            }
            if (msg.stamped) {
                msg.ts24 = node_a.timesync.LocalTimeToDatagramTS24(local_send);
                msg.app_ts23 = node_a.timesync.ToRemoteTime23(local_send);
            }

            const uint32_t delay = delay_ab.SampleDelay(now_us, rng);
            metrics.ab.sent++;
            if (!loss_ab.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ab, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ab, scenario.duplicate_delay_us, rng);
            } else {
                metrics.ab.lost++;
            }
            data_seq_ab++;
            next_data_ab = (data_interval_us > 0) ? (now_us + data_interval_us) : UINT64_MAX;
        }

        // Send data B->A
        if (next_data_ba == now_us) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            Message msg;
            msg.from_a = false;
            msg.kind = MsgKind::Data;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            msg.stamped = true;
            if ((method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) && method.sample_stride > 1) {
                msg.stamped = ((data_seq_ba % (uint64_t)method.sample_stride) == 0);
            }
            if (msg.stamped) {
                msg.ts24 = node_b.timesync.LocalTimeToDatagramTS24(local_send);
                msg.app_ts23 = node_b.timesync.ToRemoteTime23(local_send);
            }

            const uint32_t delay = delay_ba.SampleDelay(now_us, rng);
            metrics.ba.sent++;
            if (!loss_ba.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ba, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ba, scenario.duplicate_delay_us, rng);
            } else {
                metrics.ba.lost++;
            }
            data_seq_ba++;
            next_data_ba = (data_interval_us > 0) ? (now_us + data_interval_us) : UINT64_MAX;
        }

        // Send probe A->B
        if (next_probe_ab == now_us && (method.kind == MethodKind::Cristian || method.kind == MethodKind::Ntp || method.kind == MethodKind::Ptp)) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            Message msg;
            msg.from_a = true;
            msg.kind = MsgKind::ProbeReq;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.t1_local = local_send;
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);

            const uint32_t delay = delay_ab.SampleDelay(now_us, rng);
            if (!loss_ab.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ab, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ab, scenario.duplicate_delay_us, rng);
            }
            next_probe_ab = (probe_interval_us > 0) ? (now_us + probe_interval_us) : UINT64_MAX;
        }

        // Send probe B->A
        if (next_probe_ba == now_us && (method.kind == MethodKind::Cristian || method.kind == MethodKind::Ntp || method.kind == MethodKind::Ptp)) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            Message msg;
            msg.from_a = false;
            msg.kind = MsgKind::ProbeReq;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.t1_local = local_send;
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);

            const uint32_t delay = delay_ba.SampleDelay(now_us, rng);
            if (!loss_ba.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ba, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ba, scenario.duplicate_delay_us, rng);
            }
            next_probe_ba = (probe_interval_us > 0) ? (now_us + probe_interval_us) : UINT64_MAX;
        }

        // MinDelta exchange for TimeSync/Piggyback
        if (next_mindelta_ab == now_us && (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback)) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            Message msg;
            msg.from_a = true;
            msg.kind = MsgKind::MinDelta;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            if (method.kind == MethodKind::TimeSync) {
                msg.min_delta = node_a.timesync.GetMinDeltaTS24();
            } else {
                msg.env_delta_us = (uint32_t)std::max(0.0, node_a.local_env.Value());
            }
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);

            const uint32_t delay = delay_ab.SampleDelay(now_us, rng);
            if (!loss_ab.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ab, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ab, scenario.duplicate_delay_us, rng);
            }
            next_mindelta_ab = (method.mindelta_interval_us > 0) ? (now_us + method.mindelta_interval_us) : UINT64_MAX;
        }

        if (next_mindelta_ba == now_us && (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback)) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            Message msg;
            msg.from_a = false;
            msg.kind = MsgKind::MinDelta;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            if (method.kind == MethodKind::TimeSync) {
                msg.min_delta = node_b.timesync.GetMinDeltaTS24();
            } else {
                msg.env_delta_us = (uint32_t)std::max(0.0, node_b.local_env.Value());
            }
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);

            const uint32_t delay = delay_ba.SampleDelay(now_us, rng);
            if (!loss_ba.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ba, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ba, scenario.duplicate_delay_us, rng);
            }
            next_mindelta_ba = (method.mindelta_interval_us > 0) ? (now_us + method.mindelta_interval_us) : UINT64_MAX;
        }
    }

    metrics.synced_a = node_a.synced;
    metrics.synced_b = node_b.synced;
    metrics.sync_time_a_us = node_a.sync_time_us;
    metrics.sync_time_b_us = node_b.sync_time_us;
    metrics.mem_bytes = (double)(node_a.estimator.samples.size() + node_b.estimator.samples.size() +
        node_a.local_env.samples.size() + node_b.local_env.samples.size()) * sizeof(OffsetSample);
    metrics.cpu_ops = (double)(metrics.ab.sent + metrics.ba.sent + metrics.ab.received + metrics.ba.received);

    return metrics;
}

//------------------------------------------------------------------------------
// Teleop macrobench (E9)

struct TeleopState
{
    double x = 0.0;
    double v = 0.0;
};

static BenchmarkMetrics RunTeleopScenario(const ScenarioConfig& scenario_in, const MethodConfig& method_in, uint64_t seed)
{
    ScenarioConfig scenario = scenario_in;
    MethodConfig method = method_in;
    ApplyBudget(method, scenario);

    PCGRandom rng;
    rng.Seed(seed, 0xC0FFEEULL);

    BenchmarkMetrics metrics;
    metrics.overhead_bps = EstimateOverheadBps(method, scenario);

    NodeState node_a;
    NodeState node_b;

    node_a.clock.drift_ppm_base = scenario.drift_ppm_a;
    node_a.clock.offset_us = scenario.offset_a_us;
    node_a.clock.recv_noise_us = scenario.recv_noise_us;
    node_a.clock.recv_noise_mode = scenario.recv_noise_mode;
    node_a.clock.recv_noise_sigma = scenario.recv_noise_sigma;
    node_b.clock = node_a.clock;
    node_b.clock.drift_ppm_base = scenario.drift_ppm_b;
    node_b.clock.offset_us = scenario.offset_b_us;

    node_a.estimator.estimator = method.estimator;
    node_a.estimator.discipline = method.discipline;
    node_a.estimator.quantile = method.quantile;
    node_a.estimator.window_us = method.window_us;
    node_b.estimator = node_a.estimator;

    node_a.local_env.estimator = method.estimator;
    node_a.local_env.quantile = method.quantile;
    node_a.local_env.window_us = method.window_us;
    node_b.local_env = node_a.local_env;

    DelayModel delay_ab = scenario.delay_ab;
    DelayModel delay_ba = scenario.delay_ba;
    LossModel loss_ab = scenario.loss_ab;
    LossModel loss_ba = scenario.loss_ba;

    const uint64_t control_interval_us = (scenario.send_rate_hz > 0.0)
        ? (uint64_t)std::llround(1000000.0 / scenario.send_rate_hz)
        : 20000;

    uint64_t now_us = 0;
    uint64_t next_control = 0;
    uint64_t next_telemetry = 0;
    uint64_t next_mindelta_ab = method.mindelta_interval_us ? method.mindelta_interval_us : UINT64_MAX;
    uint64_t next_mindelta_ba = method.mindelta_interval_us ? method.mindelta_interval_us : UINT64_MAX;

    std::priority_queue<ArrivalEvent, std::vector<ArrivalEvent>, ArrivalCompare> arrivals;

    TeleopState plant;
    TeleopState plant_meas;

    double err_sum = 0.0;
    double err_max = 0.0;
    int err_count = 0;

    double u = 0.0;
    double desired = 0.0;

    while (now_us <= scenario.duration_us) {
        uint64_t next_arrival = arrivals.empty() ? UINT64_MAX : arrivals.top().deliver_true_us;
        uint64_t next_time = next_arrival;
        if (next_control < next_time) next_time = next_control;
        if (next_telemetry < next_time) next_time = next_telemetry;
        if (next_mindelta_ab < next_time) next_time = next_mindelta_ab;
        if (next_mindelta_ba < next_time) next_time = next_mindelta_ba;

        if (next_time == UINT64_MAX) {
            break;
        }
        now_us = next_time;

        // Advance plant to now
        const double dt = (double)control_interval_us / 1000000.0;
        plant.v += (-0.8 * plant.v + u) * dt;
        plant.x += plant.v * dt;

        // Deliver messages
        while (!arrivals.empty() && arrivals.top().deliver_true_us == now_us) {
            ArrivalEvent ev = arrivals.top();
            arrivals.pop();

            const bool to_b = ev.msg.from_a;
            NodeState& recv_node = to_b ? node_b : node_a;
            NodeState& send_node = to_b ? node_a : node_b;

            const uint64_t local_recv = ComputeLocalTimeUsec(now_us, recv_node.clock, rng, true);
            const uint64_t true_remote_local = ev.msg.true_remote_local_at_send;
            const double true_offset = (double)ev.msg.send_local_us - (double)true_remote_local;

            if (ev.msg.kind == MsgKind::TeleopState) {
                // Update estimator using timestamped telemetry
                if (method.kind == MethodKind::TimeSync) {
                    if (ev.msg.stamped) {
                        recv_node.timesync.OnAuthenticatedDatagramTimestamp(ev.msg.ts24, local_recv);
                    }
                } else if (method.kind == MethodKind::Piggyback) {
                    if (ev.msg.stamped) {
                        const double delta_us = (double)local_recv - (double)ev.msg.send_local_us;
                        recv_node.local_env.Add(now_us, delta_us);
                        if (recv_node.local_env.Ready() && recv_node.peer_env_valid) {
                            const double offset_est = (recv_node.peer_env_value - recv_node.local_env.Value()) * 0.5;
                            recv_node.estimator.AddSample(now_us, offset_est);
                        }
                    }
                } else {
                    if (recv_node.estimator.Ready()) {
                        AddOffsetError(metrics.ba, recv_node.estimator.offset_est - true_offset, now_us);
                    }
                }

                // Telemetry payload
                std::memcpy(&plant_meas.x, &ev.msg.t2_remote, sizeof(double));
                std::memcpy(&plant_meas.v, &ev.msg.t3_remote, sizeof(double));
            }
            else if (ev.msg.kind == MsgKind::TeleopCmd) {
                // Apply command
                std::memcpy(&u, &ev.msg.t2_remote, sizeof(double));
            }
            else if (ev.msg.kind == MsgKind::MinDelta) {
                if (method.kind == MethodKind::TimeSync) {
                    recv_node.timesync.OnPeerMinDeltaTS24(ev.msg.min_delta);
                } else if (method.kind == MethodKind::Piggyback) {
                    recv_node.peer_env_value = (double)ev.msg.env_delta_us;
                    recv_node.peer_env_valid = true;
                }
            }
        }

        if (next_control == now_us) {
            // Controller at A
            desired = std::sin(2.0 * 3.14159265358979323846 * (double)now_us / 1000000.0 * 0.2);

            double age_est_s = 0.0;
            if (method.kind == MethodKind::TimeSync && node_a.timesync.IsSynchronized()) {
                age_est_s = node_a.timesync.GetMinimumOneWayDelayUsec() / 1000000.0;
            } else if (node_a.estimator.Ready()) {
                age_est_s = std::max(0.0, node_a.estimator.offset_est / 1000000.0);
            }

            const double x_pred = plant_meas.x + plant_meas.v * age_est_s;
            const double v_pred = plant_meas.v;

            const double kp = 1.2;
            const double kd = 0.4;
            u = kp * (desired - x_pred) - kd * v_pred;

            // Send command to B
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            Message msg;
            msg.from_a = true;
            msg.kind = MsgKind::TeleopCmd;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            std::memcpy(&msg.t2_remote, &u, sizeof(double));

            const uint32_t delay = delay_ab.SampleDelay(now_us, rng);
            if (!loss_ab.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ab, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ab, scenario.duplicate_delay_us, rng);
            }

            // Teleop error metrics
            const double err = plant.x - desired;
            err_sum += err * err;
            err_max = std::max(err_max, std::fabs(err));
            err_count++;

            next_control = now_us + control_interval_us;
        }

        if (next_telemetry == now_us) {
            // Send telemetry from B to A
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            Message msg;
            msg.from_a = false;
            msg.kind = MsgKind::TeleopState;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            msg.stamped = true;
            if ((method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback) && method.sample_stride > 1) {
                msg.stamped = ((now_us / control_interval_us) % (uint64_t)method.sample_stride) == 0;
            }
            if (msg.stamped) {
                msg.ts24 = node_b.timesync.LocalTimeToDatagramTS24(local_send);
            }
            std::memcpy(&msg.t2_remote, &plant.x, sizeof(double));
            std::memcpy(&msg.t3_remote, &plant.v, sizeof(double));

            const uint32_t delay = delay_ba.SampleDelay(now_us, rng);
            if (!loss_ba.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ba, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ba, scenario.duplicate_delay_us, rng);
            }
            next_telemetry = now_us + control_interval_us;
        }

        if (next_mindelta_ab == now_us && (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback)) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);
            Message msg;
            msg.from_a = true;
            msg.kind = MsgKind::MinDelta;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            if (method.kind == MethodKind::TimeSync) {
                msg.min_delta = node_a.timesync.GetMinDeltaTS24();
            } else {
                msg.env_delta_us = (uint32_t)std::max(0.0, node_a.local_env.Value());
            }
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);

            const uint32_t delay = delay_ab.SampleDelay(now_us, rng);
            if (!loss_ab.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ab, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ab, scenario.duplicate_delay_us, rng);
            }
            next_mindelta_ab = (method.mindelta_interval_us > 0) ? (now_us + method.mindelta_interval_us) : UINT64_MAX;
        }

        if (next_mindelta_ba == now_us && (method.kind == MethodKind::TimeSync || method.kind == MethodKind::Piggyback)) {
            const uint64_t local_send = ComputeLocalTimeUsec(now_us, node_b.clock, rng, false);
            Message msg;
            msg.from_a = false;
            msg.kind = MsgKind::MinDelta;
            msg.send_true_us = now_us;
            msg.send_local_us = local_send;
            if (method.kind == MethodKind::TimeSync) {
                msg.min_delta = node_b.timesync.GetMinDeltaTS24();
            } else {
                msg.env_delta_us = (uint32_t)std::max(0.0, node_b.local_env.Value());
            }
            msg.true_remote_local_at_send = ComputeLocalTimeUsec(now_us, node_a.clock, rng, false);

            const uint32_t delay = delay_ba.SampleDelay(now_us, rng);
            if (!loss_ba.ShouldDrop(rng, delay)) {
                ScheduleWithEffects(arrivals, msg, now_us, delay,
                    scenario.reorder_prob_ba, scenario.reorder_advance_us, scenario.reorder_delay_us,
                    scenario.duplicate_prob_ba, scenario.duplicate_delay_us, rng);
            }
            next_mindelta_ba = (method.mindelta_interval_us > 0) ? (now_us + method.mindelta_interval_us) : UINT64_MAX;
        }
    }

    if (err_count > 0) {
        metrics.teleop_rms_error = std::sqrt(err_sum / (double)err_count);
        metrics.teleop_max_error = err_max;
    }

    metrics.cpu_ops = (double)err_count;
    return metrics;
}

//------------------------------------------------------------------------------
// Scenario and method definitions

static ScenarioConfig BaseScenario(const string& name)
{
    ScenarioConfig cfg;
    cfg.name = name;
    cfg.delay_ab.base_delay_us = 20000;
    cfg.delay_ba.base_delay_us = 20000;
    cfg.delay_ab.jitter_us = 2000;
    cfg.delay_ba.jitter_us = 2000;
    cfg.offset_a_us = 0;
    cfg.offset_b_us = 1000000;
    return cfg;
}

static std::vector<ScenarioConfig> BuildScenarios()
{
    std::vector<ScenarioConfig> sc;

    // E0 Noise floor
    {
        ScenarioConfig cfg = BaseScenario("E0_noise_floor");
        cfg.delay_ab.base_delay_us = 2000;
        cfg.delay_ba.base_delay_us = 2000;
        cfg.delay_ab.jitter_us = 0;
        cfg.delay_ba.jitter_us = 0;
        cfg.recv_noise_us = 50;
        sc.push_back(cfg);
    }

    // E1 Stationary jitter
    {
        ScenarioConfig cfg = BaseScenario("E1_stationary_jitter");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 20000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        sc.push_back(cfg);
    }

    // E2 Asymmetry sweep (representative)
    {
        ScenarioConfig cfg = BaseScenario("E2_asymmetry_40ms");
        cfg.delay_ab.base_delay_us = 20000;
        cfg.delay_ba.base_delay_us = 60000;
        sc.push_back(cfg);
    }

    // E3 Bufferbloat/heavy tail
    {
        ScenarioConfig cfg = BaseScenario("E3_bufferbloat");
        cfg.delay_ab.jitter_mode = JitterMode::Pareto;
        cfg.delay_ba.jitter_mode = JitterMode::Pareto;
        cfg.delay_ab.pareto_alpha = 1.8;
        cfg.delay_ba.pareto_alpha = 1.8;
        cfg.delay_ab.pareto_scale_us = 1000;
        cfg.delay_ba.pareto_scale_us = 1000;
        cfg.delay_ab.spike_prob = 0.01;
        cfg.delay_ba.spike_prob = 0.01;
        cfg.delay_ab.spike_delay_us = 200000;
        cfg.delay_ba.spike_delay_us = 200000;
        sc.push_back(cfg);
    }

    // E4 Loss/burst
    {
        ScenarioConfig cfg = BaseScenario("E4_loss_burst");
        cfg.loss_ab.loss_rate = 0.05;
        cfg.loss_ba.loss_rate = 0.05;
        cfg.loss_ab.burst_start_prob = 0.02;
        cfg.loss_ba.burst_start_prob = 0.02;
        cfg.loss_ab.burst_len_min = 5;
        cfg.loss_ab.burst_len_max = 20;
        cfg.loss_ba.burst_len_min = 5;
        cfg.loss_ba.burst_len_max = 20;
        sc.push_back(cfg);
    }

    // E5 Reorder/duplicate
    {
        ScenarioConfig cfg = BaseScenario("E5_reorder_dup");
        cfg.reorder_prob_ab = 0.02;
        cfg.reorder_prob_ba = 0.02;
        cfg.duplicate_prob_ab = 0.01;
        cfg.duplicate_prob_ba = 0.01;
        sc.push_back(cfg);
    }

    // E6 Path change / handover
    {
        ScenarioConfig cfg = BaseScenario("E6_path_change");
        cfg.delay_ab.step_at_us = 30000000ULL;
        cfg.delay_ab.step_delta_us = 30000;
        cfg.delay_ba.step_at_us = 30000000ULL;
        cfg.delay_ba.step_delta_us = -10000;
        sc.push_back(cfg);
    }

    // E7 Drift stress
    {
        ScenarioConfig cfg = BaseScenario("E7_drift");
        cfg.drift_ppm_a = 50;
        cfg.drift_ppm_b = -50;
        cfg.drift_step_enabled = true;
        cfg.drift_step_time_us = 20000000ULL;
        cfg.drift_step_delta_ppm_a = 100;
        cfg.drift_step_delta_ppm_b = -100;
        sc.push_back(cfg);
    }

    // E8 Overhead budget fairness
    {
        ScenarioConfig cfg = BaseScenario("E8_budget_200bps");
        cfg.overhead_budget_bps = 200.0;
        cfg.send_rate_hz = 60.0;
        sc.push_back(cfg);
    }

    // E9 Teleop macrobench
    {
        ScenarioConfig cfg = BaseScenario("E9_teleop");
        cfg.kind = ScenarioKind::Teleop;
        cfg.send_rate_hz = 50.0;
        cfg.delay_ab.base_delay_us = 30000;
        cfg.delay_ba.base_delay_us = 30000;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        sc.push_back(cfg);
    }

    // Train/holdout split (mark last 3 as holdout)
    for (size_t i = 0; i < sc.size(); ++i) {
        sc[i].train = (i < sc.size() - 3);
    }

    return sc;
}

static std::vector<MethodConfig> BuildMethodVariants(bool grid)
{
    std::vector<MethodConfig> methods;

    auto add = [&](const MethodConfig& m) {
        methods.push_back(m);
    };

    MethodConfig base;
    base.quantile = 0.01;
    base.window_us = 2 * 1000 * 1000ULL;
    base.mindelta_interval_us = 1000000;
    base.sample_stride = 1;

    // Probe-based methods
    const EstimatorKind est_list[] = {EstimatorKind::Min, EstimatorKind::Median, EstimatorKind::Regression};
    const DisciplineKind disc_list[] = {DisciplineKind::None, DisciplineKind::Hybrid, DisciplineKind::Kalman};

    auto add_probe = [&](MethodKind kind) {
        if (!grid) {
            MethodConfig m = base;
            m.kind = kind;
            m.estimator = EstimatorKind::Median;
            m.discipline = DisciplineKind::Hybrid;
            m.probe_rate_hz = 10.0;
            add(m);
            return;
        }
        for (size_t ei = 0; ei < sizeof(est_list)/sizeof(est_list[0]); ++ei) {
            for (size_t di = 0; di < sizeof(disc_list)/sizeof(disc_list[0]); ++di) {
                MethodConfig m = base;
                m.kind = kind;
                m.estimator = est_list[ei];
                m.discipline = disc_list[di];
                m.probe_rate_hz = 10.0;
                add(m);
            }
        }
    };

    add_probe(MethodKind::Cristian);
    add_probe(MethodKind::Ntp);
    add_probe(MethodKind::Ptp);

    // M4a - TimeSync
    {
        MethodConfig m = base;
        m.kind = MethodKind::TimeSync;
        m.estimator = EstimatorKind::Min;
        m.discipline = DisciplineKind::None;
        m.probe_rate_hz = 0.0;
        add(m);
    }

    // Piggyback variants (M4b/M4c)
    const double quantiles[] = {0.01, 0.05};
    const int strides[] = {1, 10};
    if (!grid) {
        MethodConfig m = base;
        m.kind = MethodKind::Piggyback;
        m.estimator = EstimatorKind::Median;
        m.discipline = DisciplineKind::Hybrid;
        add(m);
        MethodConfig m2 = base;
        m2.kind = MethodKind::Piggyback;
        m2.estimator = EstimatorKind::Quantile;
        m2.discipline = DisciplineKind::Hybrid;
        m2.sample_stride = 10;
        m2.quantile = 0.01;
        add(m2);
    } else {
        for (size_t qi = 0; qi < sizeof(quantiles)/sizeof(quantiles[0]); ++qi) {
            for (size_t si = 0; si < sizeof(strides)/sizeof(strides[0]); ++si) {
                for (size_t di = 0; di < sizeof(disc_list)/sizeof(disc_list[0]); ++di) {
                    MethodConfig m = base;
                    m.kind = MethodKind::Piggyback;
                    m.estimator = EstimatorKind::Quantile;
                    m.quantile = quantiles[qi];
                    m.sample_stride = strides[si];
                    m.discipline = disc_list[di];
                    add(m);
                }
            }
        }
    }

    return methods;
}

//------------------------------------------------------------------------------
// CSV output

static void WriteCsvHeader(std::ofstream& out)
{
    out << "scenario,train,method,estimator,discipline,seed,"
        << "offset_p50_ab_us,offset_p95_ab_us,offset_p99_ab_us,"
        << "offset_p50_ba_us,offset_p95_ba_us,offset_p99_ba_us,"
        << "skew_p95_ab_ppm,skew_p95_ba_ppm,"
        << "owd_p50_ab_us,owd_p95_ab_us,owd_p99_ab_us,"
        << "owd_p50_ba_us,owd_p95_ba_us,owd_p99_ba_us,"
        << "converge_ab_s,converge_ba_s,"
        << "deadline_tp,deadline_fp,deadline_fn,deadline_tn,"
        << "overhead_bps,cpu_ops,mem_bytes,"
        << "teleop_rms_error,teleop_max_error\n";
}

static void WriteCsvRow(std::ofstream& out, const ScenarioConfig& scenario, const MethodConfig& method, uint64_t seed, const BenchmarkMetrics& m)
{
    out << scenario.name << "," << (scenario.train ? 1 : 0) << ","
        << MethodName(method.kind) << ","
        << EstimatorName(method.estimator) << ","
        << DisciplineName(method.discipline) << ","
        << seed << ","
        << m.ab.offset_err_us.Percentile(0.50) << ","
        << m.ab.offset_err_us.Percentile(0.95) << ","
        << m.ab.offset_err_us.Percentile(0.99) << ","
        << m.ba.offset_err_us.Percentile(0.50) << ","
        << m.ba.offset_err_us.Percentile(0.95) << ","
        << m.ba.offset_err_us.Percentile(0.99) << ","
        << m.ab.skew_err_ppm.Percentile(0.95) << ","
        << m.ba.skew_err_ppm.Percentile(0.95) << ","
        << m.ab.owd_err_us.Percentile(0.50) << ","
        << m.ab.owd_err_us.Percentile(0.95) << ","
        << m.ab.owd_err_us.Percentile(0.99) << ","
        << m.ba.owd_err_us.Percentile(0.50) << ","
        << m.ba.owd_err_us.Percentile(0.95) << ","
        << m.ba.owd_err_us.Percentile(0.99) << ","
        << (m.ab.converged ? (m.ab.converge_time_us / 1000000.0) : 0.0) << ","
        << (m.ba.converged ? (m.ba.converge_time_us / 1000000.0) : 0.0) << ","
        << m.ab.deadline_tp + m.ba.deadline_tp << ","
        << m.ab.deadline_fp + m.ba.deadline_fp << ","
        << m.ab.deadline_fn + m.ba.deadline_fn << ","
        << m.ab.deadline_tn + m.ba.deadline_tn << ","
        << m.overhead_bps << ","
        << m.cpu_ops << ","
        << m.mem_bytes << ","
        << m.teleop_rms_error << ","
        << m.teleop_max_error << "\n";
}

//------------------------------------------------------------------------------
// CLI

struct CliOptions
{
    string out_csv = "peer_bench.csv";
    unsigned seeds = 5;
    bool grid = false;
    bool train_only = false;
    bool holdout_only = false;
    unsigned threads = 0;
    string scenario_filter;
    string method_filter;
    double duration_override_s = 0.0;
};

static CliOptions ParseArgs(int argc, char** argv)
{
    CliOptions opt;
    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--out") && i + 1 < argc) {
            opt.out_csv = argv[++i];
        }
        else if (!std::strcmp(argv[i], "--seeds") && i + 1 < argc) {
            opt.seeds = (unsigned)std::atoi(argv[++i]);
        }
        else if (!std::strcmp(argv[i], "--grid")) {
            opt.grid = true;
        }
        else if (!std::strcmp(argv[i], "--train")) {
            opt.train_only = true;
        }
        else if (!std::strcmp(argv[i], "--holdout")) {
            opt.holdout_only = true;
        }
        else if (!std::strcmp(argv[i], "--threads") && i + 1 < argc) {
            opt.threads = (unsigned)std::atoi(argv[++i]);
        }
        else if (!std::strcmp(argv[i], "--scenario") && i + 1 < argc) {
            opt.scenario_filter = argv[++i];
        }
        else if (!std::strcmp(argv[i], "--method") && i + 1 < argc) {
            opt.method_filter = argv[++i];
        }
        else if (!std::strcmp(argv[i], "--duration") && i + 1 < argc) {
            opt.duration_override_s = std::atof(argv[++i]);
        }
    }
    return opt;
}

//------------------------------------------------------------------------------
// Runner

static std::vector<RunItem> BuildRunItems(const CliOptions& opt)
{
    std::vector<ScenarioConfig> scenarios = BuildScenarios();
    std::vector<MethodConfig> methods = BuildMethodVariants(opt.grid);

    std::vector<RunItem> items;

    for (size_t i = 0; i < scenarios.size(); ++i) {
        if (!opt.scenario_filter.empty() && scenarios[i].name.find(opt.scenario_filter) == string::npos) {
            continue;
        }
        if (opt.train_only && !scenarios[i].train) {
            continue;
        }
        if (opt.holdout_only && scenarios[i].train) {
            continue;
        }
        if (opt.duration_override_s > 0.0) {
            scenarios[i].duration_us = (uint64_t)(opt.duration_override_s * 1000000.0);
        }
        for (size_t m = 0; m < methods.size(); ++m) {
            const string method_name = MethodName(methods[m].kind);
            if (!opt.method_filter.empty() && method_name.find(opt.method_filter) == string::npos) {
                continue;
            }
            for (unsigned s = 0; s < opt.seeds; ++s) {
                RunItem item;
                item.scenario = scenarios[i];
                item.method = methods[m];
                item.seed = (uint64_t)s;
                items.push_back(item);
            }
        }
    }

    // Shuffle to randomize method order (paired trials still share the same seed)
    PCGRandom rng;
    rng.Seed(0x51EDFACEULL, 0x1234ULL);
    for (size_t i = items.size(); i > 1; --i) {
        size_t j = (size_t)(rng.Next() % (uint32_t)i);
        std::swap(items[i - 1], items[j]);
    }

    return items;
}

int main(int argc, char** argv)
{
    CliOptions opt = ParseArgs(argc, argv);

    std::vector<RunItem> items = BuildRunItems(opt);
    if (items.empty()) {
        std::cout << "No run items" << std::endl;
        return 1;
    }

    unsigned threads = opt.threads;
    if (threads == 0) {
        threads = std::max(1u, std::thread::hardware_concurrency());
    }

    std::vector<BenchmarkMetrics> results(items.size());
    std::atomic<size_t> next_idx(0);

    auto worker = [&]() {
        for (;;) {
            size_t idx = next_idx.fetch_add(1);
            if (idx >= items.size()) {
                return;
            }
            const RunItem& item = items[idx];
            const uint64_t seed = item.seed ^ Hash64(item.scenario.name);
            if (item.scenario.kind == ScenarioKind::Teleop) {
                results[idx] = RunTeleopScenario(item.scenario, item.method, seed);
            } else {
                results[idx] = RunScenario(item.scenario, item.method, seed);
            }
        }
    };

    std::vector<std::thread> pool;
    for (unsigned t = 0; t < threads; ++t) {
        pool.emplace_back(worker);
    }
    for (size_t i = 0; i < pool.size(); ++i) {
        pool[i].join();
    }

    std::ofstream out(opt.out_csv.c_str());
    if (!out) {
        std::cerr << "Unable to open output: " << opt.out_csv << std::endl;
        return 1;
    }
    WriteCsvHeader(out);
    for (size_t i = 0; i < items.size(); ++i) {
        WriteCsvRow(out, items[i].scenario, items[i].method, items[i].seed, results[i]);
    }
    out.close();

    std::cout << "Wrote " << items.size() << " rows to " << opt.out_csv << std::endl;
    return 0;
}
