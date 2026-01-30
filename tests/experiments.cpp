#include <TimeSync/TimeSync.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
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
        return (double)Next() / (double)UINT32_MAX;
    }

    uint64_t State = 0, Inc = 0;
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

//------------------------------------------------------------------------------
// Stats

struct SampleStats
{
    void Add(uint32_t v)
    {
        samples.push_back(v);
        sum += v;
        if (v < min) {
            min = v;
        }
        if (v > max) {
            max = v;
        }
        sorted = false;
    }

    size_t Count() const
    {
        return samples.size();
    }

    double Mean() const
    {
        if (samples.empty()) {
            return 0.0;
        }
        return (double)sum / (double)samples.size();
    }

    double Percentile(double p) const
    {
        if (samples.empty()) {
            return 0.0;
        }
        EnsureSorted();
        if (p <= 0.0) {
            return samples.front();
        }
        if (p >= 1.0) {
            return samples.back();
        }
        const size_t n = samples.size();
        const size_t idx = (size_t)std::ceil(p * (double)(n - 1));
        return samples[idx];
    }

    uint32_t Min() const { return samples.empty() ? 0 : min; }
    uint32_t Max() const { return samples.empty() ? 0 : max; }

    void Reserve(size_t n)
    {
        samples.reserve(n);
    }

private:
    void EnsureSorted() const
    {
        if (sorted) {
            return;
        }
        std::sort(samples.begin(), samples.end());
        sorted = true;
    }

    mutable std::vector<uint32_t> samples;
    uint64_t sum = 0;
    uint32_t min = UINT32_MAX;
    uint32_t max = 0;
    mutable bool sorted = false;
};

//------------------------------------------------------------------------------
// Models

enum class JitterMode
{
    Uniform,
    NormalAbs
};

struct DelayModel
{
    uint32_t base_delay_us = 50000;
    uint32_t jitter_us = 1000;
    JitterMode jitter_mode = JitterMode::Uniform;
    double spike_prob = 0.0;
    uint32_t spike_delay_us = 0;
    double bimodal_prob = 0.0;
    uint32_t bimodal_delay_us = 0;
    uint32_t queue_amp_us = 0;
    uint64_t queue_period_us = 0;
    int32_t ramp_us_per_s = 0;

    uint32_t SampleDelay(uint64_t now_us, PCGRandom& rng) const
    {
        int64_t delay = base_delay_us;

        if (ramp_us_per_s != 0) {
            delay += (int64_t)((double)ramp_us_per_s * (double)now_us / 1000000.0);
        }

        if (queue_amp_us > 0 && queue_period_us > 0) {
            const double phase = 2.0 * 3.14159265358979323846 *
                                 (double)(now_us % queue_period_us) / (double)queue_period_us;
            const double q = 0.5 * (1.0 + std::sin(phase));
            delay += (int64_t)(q * (double)queue_amp_us);
        }

        if (jitter_us > 0) {
            if (jitter_mode == JitterMode::Uniform) {
                delay += RandRange(rng, 0, jitter_us);
            }
            else {
                const double j = std::fabs(RandNormal(rng) * (double)jitter_us);
                delay += (int64_t)j;
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

    bool ShouldDrop(PCGRandom& rng)
    {
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
    uint64_t sync_interval_us = 2 * 1000 * 1000ULL;
    uint64_t metrics_warmup_us = 1000 * 1000ULL;

    DelayModel delay_ab;
    DelayModel delay_ba;
    LossModel loss_ab;
    LossModel loss_ba;

    double drift_ppm_a = 0.0;
    double drift_ppm_b = 0.0;
    int64_t offset_a_us = 0;
    int64_t offset_b_us = 5 * 1000 * 1000LL;
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

static uint64_t LocalTimeUsec(uint64_t true_us, double drift_ppm, int64_t offset_us)
{
    const double scale = 1.0 + drift_ppm * 1e-6;
    const double local = (double)offset_us + (double)true_us * scale;
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

static ExperimentMetrics RunExperiment(const ExperimentConfig& cfg, uint64_t seed)
{
    PCGRandom rng;
    rng.Seed(seed, 0x12345678ULL);

    TimeSynchronizer sync_a;
    TimeSynchronizer sync_b;

    LossModel loss_ab = cfg.loss_ab;
    LossModel loss_ba = cfg.loss_ba;

    uint64_t next_data_ab = 0;
    uint64_t next_data_ba = 0;
    uint64_t next_sync_ab = cfg.sync_interval_us;
    uint64_t next_sync_ba = cfg.sync_interval_us;

    uint64_t now_us = 0;

    std::priority_queue<ArrivalEvent, std::vector<ArrivalEvent>, ArrivalCompare> arrivals;

    ExperimentMetrics metrics;
    const double expected_packets = (double)cfg.duration_us * cfg.send_rate_hz / 1000000.0;
    const size_t reserve_count = (size_t)std::ceil(expected_packets);
    metrics.ab.time_error_us.Reserve(reserve_count);
    metrics.ab.owd_error_us.Reserve(reserve_count);
    metrics.ba.time_error_us.Reserve(reserve_count);
    metrics.ba.owd_error_us.Reserve(reserve_count);

    while (now_us <= cfg.duration_us) {
        uint64_t next_arrival = arrivals.empty() ? UINT64_MAX : arrivals.top().deliver_true_us;
        uint64_t next_time = next_arrival;
        if (next_data_ab < next_time) next_time = next_data_ab;
        if (next_data_ba < next_time) next_time = next_data_ba;
        if (next_sync_ab < next_time) next_time = next_sync_ab;
        if (next_sync_ba < next_time) next_time = next_sync_ba;

        if (next_time == UINT64_MAX) {
            break;
        }

        now_us = next_time;

        // Deliver packets scheduled for now
        while (!arrivals.empty() && arrivals.top().deliver_true_us == now_us) {
            ArrivalEvent ev = arrivals.top();
            arrivals.pop();

            const bool to_b = ev.packet.from_a;
            TimeSynchronizer& recv_sync = to_b ? sync_b : sync_a;
            const double recv_drift = to_b ? cfg.drift_ppm_b : cfg.drift_ppm_a;
            const int64_t recv_offset = to_b ? cfg.offset_b_us : cfg.offset_a_us;
            DirectionMetrics& dir = to_b ? metrics.ab : metrics.ba;

            const uint64_t local_recv = LocalTimeUsec(now_us, recv_drift, recv_offset);

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
                metrics.metrics_start_us = sync_max + cfg.metrics_warmup_us;
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
                    const uint64_t true_local_at_send = LocalTimeUsec(
                        ev.packet.send_true_us,
                        recv_drift,
                        recv_offset);
                    const uint64_t estimated_local = recv_sync.FromLocalTime23(
                        local_recv,
                        ev.packet.app_ts23);
                    const uint64_t err = (estimated_local > true_local_at_send)
                        ? (estimated_local - true_local_at_send)
                        : (true_local_at_send - estimated_local);
                    dir.time_error_us.Add((uint32_t)err);
                }
            }
        }

        // Send data A->B
        if (next_data_ab == now_us) {
            const uint64_t local_send = LocalTimeUsec(now_us, cfg.drift_ppm_a, cfg.offset_a_us);

            Packet pkt;
            pkt.from_a = true;
            pkt.is_sync = false;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_a.LocalTimeToDatagramTS24(local_send);

            if (sync_a.IsSynchronized()) {
                pkt.app_ts23 = sync_a.ToRemoteTime23(local_send);
                pkt.app_ts_valid = pkt.app_ts23 != 0;
            }

            metrics.ab.sent++;

            const bool drop = loss_ab.ShouldDrop(rng);
            if (drop) {
                metrics.ab.lost++;
            }
            else {
                pkt.true_delay_us = cfg.delay_ab.SampleDelay(now_us, rng);
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);
            }

            const uint64_t interval = (uint64_t)std::llround(1000000.0 / cfg.send_rate_hz);
            next_data_ab = now_us + interval;
        }

        // Send data B->A
        if (next_data_ba == now_us) {
            const uint64_t local_send = LocalTimeUsec(now_us, cfg.drift_ppm_b, cfg.offset_b_us);

            Packet pkt;
            pkt.from_a = false;
            pkt.is_sync = false;
            pkt.send_true_us = now_us;
            pkt.ts24 = sync_b.LocalTimeToDatagramTS24(local_send);

            if (sync_b.IsSynchronized()) {
                pkt.app_ts23 = sync_b.ToRemoteTime23(local_send);
                pkt.app_ts_valid = pkt.app_ts23 != 0;
            }

            metrics.ba.sent++;

            const bool drop = loss_ba.ShouldDrop(rng);
            if (drop) {
                metrics.ba.lost++;
            }
            else {
                pkt.true_delay_us = cfg.delay_ba.SampleDelay(now_us, rng);
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);
            }

            const uint64_t interval = (uint64_t)std::llround(1000000.0 / cfg.send_rate_hz);
            next_data_ba = now_us + interval;
        }

        // Send sync A->B
        if (next_sync_ab == now_us) {
            const uint64_t local_send = LocalTimeUsec(now_us, cfg.drift_ppm_a, cfg.offset_a_us);

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

            const bool drop = loss_ab.ShouldDrop(rng);
            if (drop) {
                metrics.ab.lost++;
            }
            else {
                pkt.true_delay_us = cfg.delay_ab.SampleDelay(now_us, rng);
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);
            }

            next_sync_ab = now_us + cfg.sync_interval_us;
        }

        // Send sync B->A
        if (next_sync_ba == now_us) {
            const uint64_t local_send = LocalTimeUsec(now_us, cfg.drift_ppm_b, cfg.offset_b_us);

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

            const bool drop = loss_ba.ShouldDrop(rng);
            if (drop) {
                metrics.ba.lost++;
            }
            else {
                pkt.true_delay_us = cfg.delay_ba.SampleDelay(now_us, rng);
                ArrivalEvent ev;
                ev.deliver_true_us = now_us + pkt.true_delay_us;
                ev.packet = pkt;
                arrivals.push(ev);
            }

            next_sync_ba = now_us + cfg.sync_interval_us;
        }
    }

    metrics.min_owd_est_a_us = sync_a.GetMinimumOneWayDelayUsec();
    metrics.min_owd_est_b_us = sync_b.GetMinimumOneWayDelayUsec();

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
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_50ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 50.0;
        cfg.drift_ppm_b = -50.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_100ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 100.0;
        cfg.drift_ppm_b = -100.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_200ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 200.0;
        cfg.drift_ppm_b = -200.0;
        exps.push_back(cfg);
    }

    // Drift stress (12)
    {
        ExperimentConfig cfg = BaseConfig("drift_300ppm");
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 300.0;
        cfg.drift_ppm_b = -300.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_500ppm");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 500.0;
        cfg.drift_ppm_b = -500.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_sync5s");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.sync_interval_us = 5 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm_sync10s");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.sync_interval_us = 10 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_rate10hz");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.send_rate_hz = 10.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_rate240hz");
        cfg.duration_us = 60 * 1000 * 1000ULL;
        cfg.send_rate_hz = 240.0;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        exps.push_back(cfg);
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
        exps.push_back(cfg);
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
        exps.push_back(cfg);
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
        exps.push_back(cfg);
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
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_1000ppm_long300s");
        cfg.duration_us = 300 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 1000.0;
        cfg.drift_ppm_b = -1000.0;
        exps.push_back(cfg);
    }
    {
        ExperimentConfig cfg = BaseConfig("drift_2000ppm_long300s");
        cfg.duration_us = 300 * 1000 * 1000ULL;
        cfg.delay_ab.jitter_us = 5000;
        cfg.delay_ba.jitter_us = 5000;
        cfg.drift_ppm_a = 2000.0;
        cfg.drift_ppm_b = -2000.0;
        exps.push_back(cfg);
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
        exps.push_back(cfg);
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

//------------------------------------------------------------------------------
// CLI helpers

struct CliOptions
{
    bool list_only = false;
    string csv_path = "experiments.csv";
    std::vector<string> only;
    string match;
    uint64_t seed = 0xC0FFEEULL;
    unsigned threads = 1;
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
            if (opt.threads == 0) {
                opt.threads = 1;
            }
        }
        else if (std::strcmp(arg, "--help") == 0 || std::strcmp(arg, "-h") == 0) {
            std::cout << "Usage: experiments [--list] [--csv path] [--only name] [--match substring] [--seed n] [--threads n]\n";
            std::exit(0);
        }
    }
    return opt;
}

static void WriteCsvHeader(std::ofstream& out)
{
    out << "name,duration_s,send_rate_hz,sync_interval_s,base_ab_ms,base_ba_ms,jitter_ab_ms,jitter_ba_ms,";
    out << "loss_ab,loss_ba,burst_start,burst_len_min,burst_len_max,spike_prob,spike_delay_ms,bimodal_prob,bimodal_delay_ms,";
    out << "queue_amp_ms,queue_period_s,ramp_ms_per_s,drift_a_ppm,drift_b_ppm,offset_a_s,offset_b_s,";
    out << "sent_ab,recv_ab,lost_ab,sent_ba,recv_ba,lost_ba,sync_time_a_s,sync_time_b_s,metrics_start_s,";
    out << "time_err_mean_ab_us,time_err_p95_ab_us,time_err_p99_ab_us,time_err_max_ab_us,time_err_count_ab,";
    out << "time_err_mean_ba_us,time_err_p95_ba_us,time_err_p99_ba_us,time_err_max_ba_us,time_err_count_ba,";
    out << "owd_err_mean_ab_us,owd_err_p95_ab_us,owd_err_p99_ab_us,owd_err_max_ab_us,owd_err_count_ab,";
    out << "owd_err_mean_ba_us,owd_err_p95_ba_us,owd_err_p99_ba_us,owd_err_max_ba_us,owd_err_count_ba,";
    out << "min_owd_est_a_us,min_owd_est_b_us,min_owd_true_avg_us";
    out << "\n";
}

static void WriteCsvRow(std::ofstream& out, const ExperimentConfig& cfg, const ExperimentMetrics& m)
{
    const double duration_s = (double)cfg.duration_us / 1000000.0;
    const double sync_interval_s = (double)cfg.sync_interval_us / 1000000.0;

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
    const double time_p95_ab = m.ab.time_error_us.Percentile(0.95);
    const double time_p95_ba = m.ba.time_error_us.Percentile(0.95);
    const double owd_p95_ab = m.ab.owd_error_us.Percentile(0.95);
    const double owd_p95_ba = m.ba.owd_error_us.Percentile(0.95);

    std::cout << cfg.name << ": "
              << "syncA=" << (m.sync_a ? "yes" : "no")
              << " syncB=" << (m.sync_b ? "yes" : "no")
              << " time_p95_ab_us=" << time_p95_ab
              << " time_p95_ba_us=" << time_p95_ba
              << " owd_p95_ab_us=" << owd_p95_ab
              << " owd_p95_ba_us=" << owd_p95_ba
              << " samples_ab=" << m.ab.time_error_us.Count()
              << " samples_ba=" << m.ba.time_error_us.Count()
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

int main(int argc, char** argv)
{
    CliOptions opt = ParseArgs(argc, argv);

    const std::vector<ExperimentConfig> experiments = BuildExperiments();

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

    std::vector<size_t> run_ids;
    run_ids.reserve(experiments.size());
    for (size_t i = 0; i < experiments.size(); ++i) {
        if (ShouldRun(experiments[i], opt)) {
            run_ids.push_back(i);
        }
    }

    const size_t run_count = run_ids.size();
    if (run_count == 0) {
        std::cerr << "No experiments selected.\n";
        return 1;
    }

    if (opt.threads > run_count) {
        opt.threads = (unsigned)run_count;
    }

    std::vector<ExperimentMetrics> results(run_count);
    std::atomic<size_t> next_index(0);

    auto worker = [&]() {
        for (;;) {
            const size_t idx = next_index.fetch_add(1);
            if (idx >= run_count) {
                break;
            }
            const ExperimentConfig& cfg = experiments[run_ids[idx]];
            const uint64_t seed = opt.seed ^ Hash64(cfg.name);
            results[idx] = RunExperiment(cfg, seed);
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(opt.threads);
    for (unsigned i = 0; i < opt.threads; ++i) {
        threads.emplace_back(worker);
    }
    for (size_t i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    WriteCsvHeader(csv);
    for (size_t i = 0; i < run_count; ++i) {
        const ExperimentConfig& cfg = experiments[run_ids[i]];
        WriteCsvRow(csv, cfg, results[i]);
        PrintSummary(cfg, results[i]);
    }

    std::cout << "Ran " << run_count << " experiments. CSV: " << opt.csv_path << "\n";

    return 0;
}
