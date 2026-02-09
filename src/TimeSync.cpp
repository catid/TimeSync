/** \file
    \brief TimeSync: Time Synchronization
    \copyright Copyright (c) 2017-2019 Christopher A. Taylor.  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of TimeSync nor the names of its contributors may be
      used to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#include <TimeSync/TimeSync.h>

#include <algorithm>
#include <cmath>
#include <vector>


//------------------------------------------------------------------------------
// WindowedMinTS24

void WindowedMinTS24::Update(
    Counter24 value,
    uint64_t timestamp,
    const uint64_t windowLengthTime)
{
    const Sample sample(value, timestamp);

    // On the first sample, new best sample, or if window length has expired:
    if (!IsValid() ||
        value <= Samples[0].Value ||
        Samples[2].TimeoutExpired(sample.Timestamp, windowLengthTime))
    {
        Reset(sample);
        return;
    }

    // Insert the new value into the sorted array
    if (value <= Samples[1].Value)
        Samples[2] = Samples[1] = sample;
    else if (value <= Samples[2].Value)
        Samples[2] = sample;

    // Expire best if it has been the best for a long time
    if (Samples[0].TimeoutExpired(sample.Timestamp, windowLengthTime))
    {
        // Also expire the next best if needed
        if (Samples[1].TimeoutExpired(sample.Timestamp, windowLengthTime))
        {
            Samples[0] = Samples[2];
            Samples[1] = sample;
        }
        else
        {
            Samples[0] = Samples[1];
            Samples[1] = Samples[2];
        }
        Samples[2] = sample;
        return;
    }

    // Quarter of window has gone by without a better value - Use the second-best
    if (Samples[1].Value == Samples[0].Value &&
        Samples[1].TimeoutExpired(sample.Timestamp, windowLengthTime / 4))
    {
        Samples[2] = Samples[1] = sample;
        return;
    }

    // Half the window has gone by without a better value - Use the third-best one
    if (Samples[2].Value == Samples[1].Value &&
        Samples[2].TimeoutExpired(sample.Timestamp, windowLengthTime / 2))
    {
        Samples[2] = sample;
    }
}

//------------------------------------------------------------------------------
// WindowedQuantileTS24

void WindowedQuantileTS24::Update(
    Counter24 value,
    uint64_t timestamp,
    const uint64_t windowLengthTime)
{
    Samples.push_back(Sample(value, timestamp));
    const uint64_t cutoff = (timestamp > windowLengthTime) ? (timestamp - windowLengthTime) : 0;
    while (!Samples.empty() && Samples.front().Timestamp < cutoff) {
        Samples.pop_front();
    }
}

Counter24 WindowedQuantileTS24::GetQuantile(double quantile) const
{
    if (Samples.empty()) {
        return Counter24(0);
    }
    double q = quantile;
    if (q < 0.0) {
        q = 0.0;
    } else if (q > 1.0) {
        q = 1.0;
    }
    std::vector<Counter24> values;
    values.reserve(Samples.size());
    for (const auto& sample : Samples) {
        values.push_back(sample.Value);
    }
    size_t idx = 0;
    if (values.size() > 1) {
        idx = (size_t)std::floor(q * (values.size() - 1));
    }
    // Note: Counter24::operator< uses modular comparison.  This is correct
    // for time-sync deltas which are small offsets near zero, but would
    // mis-order values spanning more than half the 24-bit range (~67 seconds
    // in timestamp units).  The windowed design ensures values stay close.
    std::nth_element(values.begin(), values.begin() + idx, values.end(),
        [](const Counter24& a, const Counter24& b) { return a < b; });
    return values[idx];
}


//------------------------------------------------------------------------------
// TimeSynchronizer

void TimeSynchronizer::OnPeerMinDeltaTS24(Counter24 minDeltaTS24)
{
    std::lock_guard<std::mutex> lock(Mutex);
    LastFC_MinDeltaTS24 = minDeltaTS24;
    GotPeerUpdate = true;

    RecalculateLocked();
}

void TimeSynchronizer::OnPeerSlopeEstimate(double peerSlopePpm)
{
    std::lock_guard<std::mutex> lock(Mutex);
    PeerSlopePpm_ = peerSlopePpm;
    GotPeerSlope_ = true;

    // CSE: skew = (peer_slope - local_slope) / 2
    // Both slopes measure how the recv-send delta changes over time.
    // The local slope = network_trend + skew, peer slope = network_trend - skew.
    // Subtracting: peer - local = -2*skew, so skew = (local - peer) / 2.
    if (SkewBufferCount_ >= kSkewMinSamples) {
        const double cse_candidate = (SkewEstPpm_ - PeerSlopePpm_) * 0.5;
        // Low-pass filter to smooth out noise
        CseSkewPpm_ = (1.0 - kCseBeta) * CseSkewPpm_ + kCseBeta * cse_candidate;
    }
}

double TimeSynchronizer::GetLocalSlopeForPeer() const
{
    std::lock_guard<std::mutex> lock(Mutex);
    if (!SkewCorrectionEnabled_ || SkewBufferCount_ < kSkewMinSamples) {
        return 0.0;
    }
    return SkewEstPpm_;
}

void TimeSynchronizer::Reset()
{
    std::lock_guard<std::mutex> lock(Mutex);
    Synchronized.store(false);
    RemoteTimeDeltaUsec.store(0);
    MinimumOneWayDelayUsec.store(kDefaultOWDUsec);
    WindowedMinTS24Deltas = WindowedMinTS24();
    WindowedQuantileTS24Deltas.Reset();
    LastFC_MinDeltaTS24 = 0;
    GotPeerUpdate = false;
    PeerSlopePpm_ = 0.0;
    GotPeerSlope_ = false;
    CseSkewPpm_ = 0.0;
    ResetSkewEstimatorLocked();
}

unsigned TimeSynchronizer::OnAuthenticatedDatagramTimestamp(
    Counter24 remoteSendTS24,
    uint64_t localRecvUsec)
{
    std::lock_guard<std::mutex> lock(Mutex);

    const Counter24 localTS24 = (uint32_t)(localRecvUsec >> kTime23LostBits);

    // OWD_i + ClockDelta(L-R)_i = Local Receive Time - Remote Send Time
    const Counter24 deltaTS24 = localTS24 - remoteSendTS24;

    WindowedMinTS24Deltas.Update(deltaTS24, localRecvUsec, DriftWindowUsec.load());
    WindowedQuantileTS24Deltas.Update(deltaTS24, localRecvUsec, DriftWindowUsec.load());

    RecalculateLocked(localRecvUsec);

    // Estimated one-way-delay (OWD) for this datagram in microseconds.
    // This does not include processing time only network delay and perhaps
    // some delays from the Operating System when it is heavily loaded.
    // Set to 0 if trip time is not available
    unsigned networkTripUsec = 0;

    if (IsSynchronized())
    {
        // This is equivalent to the shortest RTT/2 seen so far by any pair of packets,
        // meaning that it is the average of the upstream and downstream OWD.
        networkTripUsec = GetMinimumOneWayDelayUsec();

        // While the OWD is an estimate, the relative delay between that
        // smallest packet pair and the current datagram is actually precise:
        const Counter24 minDeltaTS24 = GetMinDeltaTS24Locked();
        if (deltaTS24 > minDeltaTS24)
        {
            const Counter24 relativeTS24 = deltaTS24 - minDeltaTS24;
            networkTripUsec += relativeTS24.ToUnsigned() << kTime23LostBits;
        }

        // What should happen here is if the delay of each packet varies a lot, then we should
        // get pretty accurate OWD for each packet.  But if the variance is low and the delays
        // for upstream and downstream are asymmetric, then it will underestimate the OWD by
        // half of that asymmetry.  Hopefully this inaccuracy won't cause problems..
    }

    return networkTripUsec;
}

void TimeSynchronizer::RecalculateLocked(uint64_t localRecvUsec)
{
    if ((MinQuantile > 0.0 ? !WindowedQuantileTS24Deltas.IsValid() : !WindowedMinTS24Deltas.IsValid()) ||
        !GotPeerUpdate)
    {
        return;
    }

    // min(OWD_i) + ClockDelta(L-R)_i
    const Counter24 minRecvDeltaTS24 = (MinQuantile > 0.0)
        ? WindowedQuantileTS24Deltas.GetQuantile(MinQuantile)
        : WindowedMinTS24Deltas.GetBest();

    // min(OWD_j) + ClockDelta(R-L)_j
    const Counter24 minSendDeltaTS24 = LastFC_MinDeltaTS24;

    // Assume min(OWD_i) = min(OWD_j):
    // min(OWD) ~= (min(OWD_j) + min(OWD_i)) / 2
    const Counter23 minOWD_TS23 = (minSendDeltaTS24 + minRecvDeltaTS24).ToUnsigned() >> 1;

    // Assume ClockDelta(R-L)_j = -ClockDelta(L-R)_i:
    // ClockDelta(R-L) ~= (ClockDelta(R-L)_j - ClockDelta(L-R)_i) / 2
    const Counter23 clockDelta_TS23 = (minSendDeltaTS24 - minRecvDeltaTS24).ToUnsigned() >> 1;

    // Calculate the raw time delta in microseconds
    const uint32_t rawOffsetUsec = clockDelta_TS23.ToUnsigned() << kTime23LostBits;

    // Apply skew correction if enabled and we have a valid timestamp
    if (SkewCorrectionEnabled_ && localRecvUsec > 0)
    {
        // Initialize base timestamp on first sample
        if (SkewBaseTimestamp_ == 0) {
            SkewBaseTimestamp_ = localRecvUsec;
            SkewLastSampleTimestamp_ = 0; // Force first sample
        }

        const double current_time_s = (double)(localRecvUsec - SkewBaseTimestamp_) * 1e-6;

        // Unwrap the raw offset to get a continuous signal for regression
        const double raw_us = (double)rawOffsetUsec;
        double unwrapped_us = raw_us + SkewUnwrapOffset_;

        if (SkewHasLastRaw_) {
            const double diff = raw_us - LastRawOffsetUsec_;
            static const double kRange = (double)(1u << 26); // Counter23 range in usec
            static const double kHalfRange = kRange / 2.0;
            if (diff > kHalfRange) {
                SkewUnwrapOffset_ -= kRange;
                unwrapped_us = raw_us + SkewUnwrapOffset_;
            } else if (diff < -kHalfRange) {
                SkewUnwrapOffset_ += kRange;
                unwrapped_us = raw_us + SkewUnwrapOffset_;
            }
        }
        LastRawOffsetUsec_ = raw_us;
        SkewHasLastRaw_ = true;

        // Step-change detection: if we have a regression and the residual is
        // too large, reset the estimator (path change, NAT rebind, etc.)
        if (SkewBufferCount_ >= kSkewMinSamples) {
            const double predicted = SkewIntercept_ + SkewEstPpm_ * current_time_s;
            if (std::abs(unwrapped_us - predicted) > StepThresholdUsec_) {
                ResetSkewEstimatorLocked();
                SkewBaseTimestamp_ = localRecvUsec;
                SkewLastSampleTimestamp_ = 0;
                unwrapped_us = raw_us;
                SkewHasLastRaw_ = false;
            }
        }

        // Add sample to ring buffer at spaced intervals.
        // This prevents the buffer from filling with identical values
        // when packets arrive faster than the minimum filter updates.
        const bool shouldSample =
            (SkewLastSampleTimestamp_ == 0) ||
            (localRecvUsec - SkewLastSampleTimestamp_ >= kSkewSampleIntervalUsec);

        if (shouldSample) {
            const double time_s = (double)(localRecvUsec - SkewBaseTimestamp_) * 1e-6;
            SkewBuffer_[SkewBufferHead_] = {time_s, unwrapped_us};
            SkewBufferHead_ = (SkewBufferHead_ + 1) % kSkewBufferSize;
            if (SkewBufferCount_ < kSkewBufferSize) {
                ++SkewBufferCount_;
            }
            SkewLastSampleTimestamp_ = localRecvUsec;

            // Refit regression when new sample is added
            if (SkewBufferCount_ >= kSkewMinSamples) {
                ComputeTheilSenSkewLocked();
            }
        }

        // Apply slope-only forward projection.
        // The raw offset from the minimum filter is accurate at the time the
        // minimum was captured, but lags behind the true offset under drift.
        // We correct by projecting forward using the estimated skew from the
        // minimum's capture time to now.
        if (SkewBufferCount_ >= kSkewMinSamples && WindowedMinTS24Deltas.IsValid()) {
            const uint64_t minTimestamp = WindowedMinTS24Deltas.GetBestTimestamp();
            const double dt_s = (double)(localRecvUsec - minTimestamp) * 1e-6;

            // Use CSE skew if available (more accurate), otherwise local estimate
            const double skew_for_correction = GotPeerSlope_ ? CseSkewPpm_ : SkewEstPpm_;

            // Only project forward (dt_s >= 0) and only for reasonable durations
            if (dt_s > 0.0 && dt_s < 30.0) {
                const double correction_us = skew_for_correction * dt_s;
                const double corrected_us = (double)rawOffsetUsec + correction_us;
                RemoteTimeDeltaUsec = (uint32_t)((int64_t)std::round(corrected_us) & 0xFFFFFFFF);
            } else {
                RemoteTimeDeltaUsec = rawOffsetUsec;
            }
        } else {
            RemoteTimeDeltaUsec = rawOffsetUsec;
        }
    }
    else
    {
        RemoteTimeDeltaUsec = rawOffsetUsec;
    }

    // Calculate the minimum OWD, which may go negative and blow up..
    uint32_t min_owd_usec = minOWD_TS23.ToUnsigned() << kTime23LostBits;

    // If the implied subtraction went negative (wrapped around), correct to zero.
    // The threshold is half the Counter23 range: (1<<22)<<3 = 33,554,432 usec (~33.5s).
    // Any computed OWD above this is treated as a wrapped negative value.
    // Note: This also means genuinely high OWD (>33.5s, e.g. extreme satellite
    // bufferbloat) will be clamped to 0.  Such conditions are outside the
    // design envelope of this library.
    static const uint32_t kSignRolloverThreshold = (1 << 22) << kTime23LostBits;
    if (min_owd_usec >= kSignRolloverThreshold) {
        min_owd_usec = 0;
    }
    MinimumOneWayDelayUsec = min_owd_usec;

    Synchronized = true;
}

void TimeSynchronizer::ComputeTheilSenSkewLocked()
{
    const unsigned n = std::min(SkewBufferCount_, kSkewBufferSize);
    if (n < kSkewMinSamples) {
        return;
    }

    // Collect all pairwise slopes
    std::vector<double> slopes;
    slopes.reserve(n * (n - 1) / 2);

    for (unsigned i = 0; i < n; ++i) {
        const unsigned idx_i = (SkewBufferHead_ + kSkewBufferSize - n + i) % kSkewBufferSize;
        for (unsigned j = i + 1; j < n; ++j) {
            const unsigned idx_j = (SkewBufferHead_ + kSkewBufferSize - n + j) % kSkewBufferSize;
            const double dt = SkewBuffer_[idx_j].time_s - SkewBuffer_[idx_i].time_s;
            if (dt > 0.001) { // Minimum 1ms separation to avoid noise
                const double doff = SkewBuffer_[idx_j].offset_us - SkewBuffer_[idx_i].offset_us;
                slopes.push_back(doff / dt); // us/s = ppm
            }
        }
    }

    if (slopes.size() < 3) {
        return;
    }

    // Median of slopes (Theil-Sen estimator)
    const size_t mid = slopes.size() / 2;
    std::nth_element(slopes.begin(), slopes.begin() + mid, slopes.end());
    SkewEstPpm_ = slopes[mid];

    // Compute intercept: median of (offset_i - slope * time_i)
    std::vector<double> intercepts;
    intercepts.reserve(n);
    for (unsigned i = 0; i < n; ++i) {
        const unsigned idx = (SkewBufferHead_ + kSkewBufferSize - n + i) % kSkewBufferSize;
        intercepts.push_back(SkewBuffer_[idx].offset_us - SkewEstPpm_ * SkewBuffer_[idx].time_s);
    }
    const size_t imid = intercepts.size() / 2;
    std::nth_element(intercepts.begin(), intercepts.begin() + imid, intercepts.end());
    SkewIntercept_ = intercepts[imid];
}

void TimeSynchronizer::ResetSkewEstimatorLocked()
{
    SkewBufferCount_ = 0;
    SkewBufferHead_ = 0;
    SkewBaseTimestamp_ = 0;
    SkewLastSampleTimestamp_ = 0;
    SkewEstPpm_ = 0.0;
    SkewIntercept_ = 0.0;
    LastRawOffsetUsec_ = 0.0;
    SkewUnwrapOffset_ = 0.0;
    SkewHasLastRaw_ = false;
}
