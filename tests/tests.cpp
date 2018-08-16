/** \file
    \brief TimeSync: Time Synchronization
    \copyright Copyright (c) 2017 Christopher A. Taylor.  All rights reserved.

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

#include "../TimeSync.h"

#include <iostream>
using namespace std;


//------------------------------------------------------------------------------
// Portability macros

// Compiler-specific debug break
#if defined(_DEBUG) || defined(DEBUG)
    #define TIMESYNC_DEBUG
    #ifdef _WIN32
        #define TIMESYNC_DEBUG_BREAK() __debugbreak()
    #else
        #define TIMESYNC_DEBUG_BREAK() __builtin_trap()
    #endif
    #define TIMESYNC_DEBUG_ASSERT(cond) { if (!(cond)) { TIMESYNC_DEBUG_BREAK(); } }
#else
    #define TIMESYNC_DEBUG_BREAK() do {} while (false);
    #define TIMESYNC_DEBUG_ASSERT(cond) do {} while (false);
#endif


//------------------------------------------------------------------------------
// Constants

#define TIMESYNC_RET_FAIL -1
#define TIMESYNC_RET_SUCCESS 0


//------------------------------------------------------------------------------
// PCG PRNG

/// From http://www.pcg-random.org/
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

    uint64_t State = 0, Inc = 0;
};


//------------------------------------------------------------------------------
// Test: Simple usage example

bool TestSimpleUsage()
{
    cout << "TestSimpleUsage...";

    TimeSynchronizer sync_a, sync_b;

    // Simulate sending UDP/IP packets back and forth between two hosts A, B.
    // Each packet contains both data and a header containing a 3 byte timestamp.

    // TODO

    cout << "Success!" << endl;

    return true;
}


//------------------------------------------------------------------------------
// Test: Simple two-round protocol

static bool is_near(
    unsigned x, // Input value x
    unsigned y, // Input value y
    unsigned limit, // Max difference
    unsigned& deltaOut) // Output parameter: Delta between them
{
    deltaOut = (x > y) ? x - y : y - x;
    return deltaOut <= limit;
}

static bool test_two_rounds(const uint64_t clock_delta, unsigned owdUsec)
{
    TimeSynchronizer sync_a, sync_b;

    // Simulate sending UDP/IP packets back and forth between two hosts A, B.
    // Each packet contains both data and a header containing a 3 byte timestamp.

    bool is_a_synched = false;
    bool is_b_synched = false;

    is_a_synched = sync_a.IsSynchronized();
    is_b_synched = sync_b.IsSynchronized();

    if (is_a_synched || is_b_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    // Define a global clock between both peers.
    // Peer A uses the global clock exactly.
    // Peer B started their computer earlier, so their clock is ahead by clock_delta.
    uint64_t globalUsec = 0;

    // Local time at peer A or B
    uint64_t localUsec_a = 0;
    uint64_t localUsec_b = 0;

    // Packet timestamps sent
    uint32_t ts_a = 0;
    uint32_t ts_b = 0;

    // Received timestamps
    Counter24 remoteSendTS24_a = 0;
    Counter24 remoteSendTS24_b = 0;

    // Local time when timestamp and data are received
    uint64_t localRecvUsec_a = 0;
    uint64_t localRecvUsec_b = 0;

    // One-way delay when sending a packet from A to B, or from B to A
    unsigned owd_a_to_b = 0;
    unsigned owd_b_to_a = 0;

    // MinDeltaTS24 value provided by peer A or peer B
    Counter24 minDeltaTS24_a = 0;
    Counter24 minDeltaTS24_b = 0;


    // Simulate sending UDP/IP datagram from A -> B:
    // (Timestamp is in the datagram header.)

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    ts_a = sync_a.LocalTimeToDatagramTS24(localUsec_a);

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    localRecvUsec_b = localUsec_b;
    remoteSendTS24_a = ts_a;

    owd_a_to_b = sync_b.OnAuthenticatedDatagramTimestamp(remoteSendTS24_a, localRecvUsec_b);
    is_b_synched = sync_b.IsSynchronized();

    if (owd_a_to_b != 0 || is_b_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }


    // Simulate sending UDP/IP datagram from B -> A:
    // (Timestamp is in the datagram header.)

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    ts_b = sync_b.LocalTimeToDatagramTS24(localUsec_b);

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    localRecvUsec_a = localUsec_a;
    remoteSendTS24_b = ts_b;

    owd_b_to_a = sync_a.OnAuthenticatedDatagramTimestamp(remoteSendTS24_b, localRecvUsec_a);
    is_a_synched = sync_a.IsSynchronized();

    if (owd_b_to_a != 0 || is_a_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }


    // Simulate sending UDP/IP datagram from A -> B containing MinDeltaTS24:
    // (Timestamp is in the datagram header.)

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    ts_a = sync_a.LocalTimeToDatagramTS24(localUsec_a);
    minDeltaTS24_a = sync_a.GetMinDeltaTS24();

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    localRecvUsec_b = localUsec_b;
    remoteSendTS24_a = ts_a;

    owd_a_to_b = sync_b.OnAuthenticatedDatagramTimestamp(remoteSendTS24_a, localRecvUsec_b);
    is_b_synched = sync_b.IsSynchronized();

    if (owd_a_to_b != 0 || is_b_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    // Should now be synced
    sync_b.OnPeerMinDeltaTS24(minDeltaTS24_a);
    is_b_synched = sync_b.IsSynchronized();
    if (!is_b_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }


    // Simulate sending UDP/IP datagram from B -> A containing MinDeltaTS24:
    // (Timestamp is in the datagram header.)

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    ts_b = sync_b.LocalTimeToDatagramTS24(localUsec_b);
    minDeltaTS24_b = sync_b.GetMinDeltaTS24();

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    localRecvUsec_a = localUsec_a;
    remoteSendTS24_b = ts_b;

    owd_b_to_a = sync_a.OnAuthenticatedDatagramTimestamp(remoteSendTS24_b, localRecvUsec_a);
    is_a_synched = sync_a.IsSynchronized();

    if (owd_b_to_a != 0 || is_a_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    // Should now be synced
    sync_a.OnPeerMinDeltaTS24(minDeltaTS24_b);
    is_a_synched = sync_a.IsSynchronized();
    if (!is_a_synched) {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }


    // Minimum OWD seen by peer A and B
    uint32_t min_owd_a = 0;
    uint32_t min_owd_b = 0;

    min_owd_a = sync_a.GetMinimumOneWayDelayUsec();
    min_owd_b = sync_b.GetMinimumOneWayDelayUsec();

    unsigned delta_a = 0;
    unsigned delta_b = 0;

    if (!is_near(min_owd_a, owdUsec, kTime23ErrorBound, delta_a) ||
        !is_near(min_owd_b, owdUsec, kTime23ErrorBound, delta_b))
    {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    // Remote time calculated by peer A or B using 16 bits
    // e.g. Peer A calculates what it thinks the time is at peer B
    uint16_t remote_time_a_16 = 0;
    uint16_t remote_time_b_16 = 0;

    // Remote time calculated by peer A or B using 23 bits
    // e.g. Peer A calculates what it thinks the time is at peer B
    uint32_t remote_time_a_23 = 0;
    uint32_t remote_time_b_23 = 0;

    // Recovered time on the receiver after receiving a 16-bit or 23-bit timestamp
    uint64_t expected_time_a = 0;
    uint64_t expected_time_b = 0;

    // Recovered time on the receiver after receiving a 16-bit or 23-bit timestamp
    uint64_t recovered_time_a = 0;
    uint64_t recovered_time_b = 0;

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    expected_time_a = localUsec_a;
    expected_time_b = localUsec_b;

    remote_time_a_16 = sync_a.ToRemoteTime16(localUsec_a);
    remote_time_b_16 = sync_b.ToRemoteTime16(localUsec_b);

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    recovered_time_a = sync_a.FromLocalTime16(localUsec_a, remote_time_b_16);
    recovered_time_b = sync_b.FromLocalTime16(localUsec_b, remote_time_a_16);

    if (!is_near(expected_time_a, recovered_time_a, kTime16ErrorBound, delta_a) ||
        !is_near(expected_time_b, recovered_time_b, kTime16ErrorBound, delta_b))
    {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    expected_time_a = localUsec_a;
    expected_time_b = localUsec_b;

    remote_time_a_23 = sync_a.ToRemoteTime23(localUsec_a);
    remote_time_b_23 = sync_b.ToRemoteTime23(localUsec_b);

    // Advance global clock
    globalUsec += owdUsec;
    localUsec_a = globalUsec;
    localUsec_b = globalUsec + clock_delta;

    recovered_time_a = sync_a.FromLocalTime23(localUsec_a, remote_time_b_23);
    recovered_time_b = sync_b.FromLocalTime23(localUsec_b, remote_time_a_23);

    if (!is_near(expected_time_a, recovered_time_a, kTime23ErrorBound, delta_a) ||
        !is_near(expected_time_b, recovered_time_b, kTime23ErrorBound, delta_b))
    {
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    return true;
}

bool TestTwoRounds()
{
    cout << "TestTwoRounds...";

    static const unsigned kTrials = 1000000;

    PCGRandom prng;
    prng.Seed(1000);

    for (unsigned i = 0; i < kTrials; ++i)
    {
        const uint64_t clock_delta = prng.Next();

        const unsigned owdUsec = (prng.Next() % 200000) + 2000; // 2..202 ms

        if (!test_two_rounds(clock_delta, owdUsec))
        {
            cout << "Failed for i = " << i << " clock_delta = " << clock_delta << endl;
            TIMESYNC_DEBUG_BREAK();
            return false;
        }
    }

    cout << "Success!" << endl;

    return true;
}


//------------------------------------------------------------------------------
// Test: Windowed minimum class

bool TestWindowedMinTS24()
{
    cout << "TestWindowedMinTS24...";

    WindowedMinTS24 window;

    Counter24 value;
    uint64_t timestamp;
    const uint64_t windowLengthTime = 100;
    const unsigned kTrials = 10;

    for (unsigned i = 0; i < kTrials * windowLengthTime; ++i)
    {
        timestamp = i;
        value = i;
        window.Update(value, timestamp, windowLengthTime);
        const unsigned smallest = window.GetBest().ToUnsigned();

        const unsigned delta = i - smallest;
        if (i <= 100)
        {
            if (smallest > 1)
            {
                cout << "Failed error too high during initial step up: i=" << i << " -> " << smallest << " back by " << (i - smallest) << endl;
                TIMESYNC_DEBUG_BREAK();
                return false;
            }
        }
        else
        {
            const unsigned error = 100 - delta;
            if (error > 50)
            {
                cout << "Failed error too high during step up: i=" << i << " -> " << smallest << " back by " << (i - smallest) << endl;
                TIMESYNC_DEBUG_BREAK();
                return false;
            }
        }
    }

    window.Reset();
    if (0 != window.GetBest().ToUnsigned())
    {
        cout << "Failed: Reset does not produce 0" << endl;
        TIMESYNC_DEBUG_BREAK();
        return false;
    }

    for (unsigned i = kTrials * windowLengthTime; i > 0; --i)
    {
        timestamp = i;
        value = i;
        window.Update(value, timestamp, windowLengthTime);
        const unsigned smallest = window.GetBest().ToUnsigned();
        if (smallest != i)
        {
            cout << "Failed error too high during step down: i=" << i << " -> " << smallest << " back by " << (i - smallest) << endl;
            TIMESYNC_DEBUG_BREAK();
            return false;
        }
    }

    cout << "Success!" << endl;

    return true;
}


//------------------------------------------------------------------------------
// Entrypoint

int main()
{
    cout << "Unit tester for TimeSync.  Exits with -1 on failure, 0 on success" << endl;
    cout << endl;

    int result = TIMESYNC_RET_SUCCESS;

    if (!TestSimpleUsage()) {
        result = TIMESYNC_RET_FAIL;
    }
    if (!TestTwoRounds()) {
        result = TIMESYNC_RET_FAIL;
    }
    if (!TestWindowedMinTS24()) {
        result = TIMESYNC_RET_FAIL;
    }

    cout << endl;
    if (result == TIMESYNC_RET_FAIL) {
        cout << "*** Tests failed (see above)!  Returning -1" << endl;
    }
    else {
        cout << "*** Tests succeeded!  Returning 0" << endl;
    }

    return result;
}
