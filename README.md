## TimeSync

TimeSync: Time Synchronization library in C++

### Motivation:

This library provides a single-header implementation of network time synchronization, suitable for synchronizing clocks between two mobile phones (iOS or Android), Linux, Windows, or Mac computers using UDP/IP sockets.  It provides a new method for time synchronization that provides several advantages over prior implementations:

(1) This new time synchronization works better over cellular networks than
PTP/NTP and is more accurate in general due to its novel N:M probing approach
(see Algorithm section below).

(2) The API provides time synchronization as a feature for applications,
enabling millisecond-accurate dead reckoning for multiplayer games with just
16-bit (2 byte) timestamps, and 16-microsecond-precise timing for scientific applications
with 23-bit (3 byte) timestamps.  This is in contrast with typical timestamps that are
between 4 and 8 bytes.

(3) The API provides network trip time for every UDP datagram that arrives.

(4) Delay-based Congestion Control systems should use One Way Delay (OWD)
on each packet as a signal, which allows it to e.g. avoid causing latency
in realtime games while delivering a file transfer in the background.
All existing Delay-based CC algorithms use differential OWD rather than
proper time synchronization.  By adding time synchronization, CC becomes
robust to changes in the base OWD as the end-points remain synced.

(5) Peer2Peer NAT hole-punch can be optimized because it can use time
synchronization to initiate probes simultaneously on both peers.

### Usage:

This is a bit more of a cookbook than an API to use.

Ingredients:

(1) Microsecond-accurate 64-bit timestamp.  I recommend the portable implementation here:
https://github.com/catid/siamese/blob/master/SiameseTools.cpp#L52

(2) (Highly Recommended) UDP/IP sockets.  You can use TCP (or other reliable-in-order sockets) for time synchronization but the quality will be much lower than with UDP due to the extra delays.  I recommend the portable Asio library that is actually stand-alone (boost not required).  To use it without boost you'll need something like this:
https://github.com/catid/mau/blob/master/thirdparty/IncludeAsio.h

(3) Include TimeSync.cpp into your project and ``#include "TimeSync.h"``.  Create a `TimeSynchronizer` object in your netcode on both the client/server code.

(4) Just before sending each UDP datagram, get the current time in microseconds `nowUsec` and call ``TimeSynchronizer::LocalTimeToDatagramTS24(nowUsec)`` to get the 24-bit (3 byte) value to attach to each outgoing UDP datagram.

(5) Periodically, each peer must call ``TimeSynchronizer::GetMinDeltaTS24()`` and send the 24-bit (3 byte) value to the remote peer.  I recommend sending this value once every 2 seconds using your reliable transport's "unordered reliable" mode if it supports that, because it is fine if they arrive out of order.  Ideally, sending the value once every 500 milliseconds for the first 20 seconds or so.  When receiving a ``MinDeltaTS24`` value, it should be passed to ``TimeSynchronizer::OnPeerMinDeltaTS24()``.  After this call, ``IsSynchronized()`` will start to return `true`.

(6) When a UDP datagram arrives, get the current time in microseconds and call ``TimeSynchronizer::OnAuthenticatedDatagramTimestamp(Counter24 remoteSendTS24, uint64_t localRecvUsec)`` with the 24-bit (3 byte) timestamp attached the datagram.

With each datagram received, the timestamp accuracy improves.

The function will return the OWD of each datagram, or 0 if ``IsSynchronized()`` is returning false.

The ``TimeSynchronizer::GetMinimumOneWayDelayUsec()`` will return the speed of light in microseconds - the smallest OWD seen so far.

To attach a timestamp for game physics or camera frames or audio or whatever the application is doing, use the ``ToRemoteTime23(uint64_t localUsec)`` and ``FromLocalTime23(uint64_t localUsec, Counter23 timestamp23)`` methods.

Call `ToRemoteTime23` with the local timestamp to send, which produces a 23-bit (3 byte) timestamp that can be sent in a UDP or TCP message.  The receiver of the message must get a current microsecond timer and can then call `FromLocalTime23(localUsec, timestamp23)` to decompress the 23-bit (3 byte) timestamp back into a 64-bit local timestamp in microseconds.  The LSB precision of 23-bit (3 byte) TS23 is 8 microseconds.  There is also a 16-bit (2 byte) TS16 version with 0.5 millisecond precision.

### Background:

Network time synchronization can be done two ways:
(a) Broadcast - Infeasible on the Internet and so not used.
(b) Assuming that the link is symmetric, and trusting Min(RTT/2) = OWD.
Meaning that existing network time synchronization protocols like NTP and
PTP work by sending multiple probes, and then taking the probe with the
smallest round trip time to be the best data to use in the set of probes.

Time synchronization at higher resolutions needs to be performed constantly
because clocks drift at a rate of about 1 millisecond per 10 seconds.
So, a common choice for reliable UDP game protocols is to probe for time
synchronization purposes (running NTP all the time) at a fixed interval of
e.g. 5 to 10 seconds.

The disadvantage of all of these existing approaches is that they only use
a finite number of probes, and all probes may be slightly skewed,
especially when jitter is present, or cross-traffic, or self-congestion
from a file transfer.

For cellular networks, existing time synchronization approaches provide
degraded results due to the 4-10 millisecond jitter on every packet.
"An End-to-End Measurement Study of Modern Cellular Data Networks" (2014)
https://www.comp.nus.edu.sg/~bleong/publications/pam14-ispcheck.pdf

when a link is asymmetrical there is no known practical method for
performing time synchronization between two peers that meet on the Internet,
so in that case a best effort is done, and at least it will be consistent.

### Algorithm:

This TimeSync protocol overcomes this jitter problem by using a massive
number of probes (every packet is a probe), greatly increasing the odds
of a minimal RTT probe.

How it works is that every packet has a 3 byte microsecond timestamp on it,
large enough to prevent roll-over.  Both sides record the receive time of
each packet as early as possible, and then throw the packet onto another
thread to process, so that the network delay is more accurate and each
client of a server can run in parallel.

While each packet is being processed the difference in send and receive
times are compared with prior such differences.  And a windowed minimum
of these differences is updated.  Periodically this minimum difference
is reported to the remote peer so that both sides have both the minimum
(outgoing - incoming) difference and the minimum (incoming - outgoing)
difference.

These minimum differences correspond to the shortest trips each way.
Effectively, it turns every single packet into a time synchronization
probe, guaranteeing that it gets the best result possible.

### The (Simple) Math for TimeSync:

We measure (Smallest C2S Delta) and (Smallest S2C Delta)
through the per-packet timestamps.

    C2S = (Smallest C2S Delta)
        = (Server Time @ Client Send Time) + (C2S Propagation Delay) - (Client Send Time)
        = (C2S Propagation Delay) + (Clock Delta)

    S2C = (Smallest S2C Delta)
        = (Client Time @ Server Send Time) + (S2C Propagation Delay) - (Server Send Time)
        = (S2C Propagation Delay) - (Clock Delta)

    Such that (Propagation Delay) for each direction is minimized
    independently as described previously.

We want to solve for (Clock Delta) but there is a problem:

Note that in the definition of C2S and S2C there are three unknowns and
only two measurements.  To resolve this problem we make the assumption
that the min. propagation delays are almost the same in each direction.

And so:

    (S2C Propagation Delay) approx equals (C2S Propagation Delay).

Thus we can simply write:

    (Clock Delta) = (C2S - S2C) / 2

This gives us 23 bits of the delta between clocks, since the division
by 2 (right shift) pulls in an unknown high bit.  The effect of any
link asymmetry is halved as a side effect, helping to minimize it.

### The (Simple) Math for Network Trip Time:

It also provides a robust estimate of the "speed of light" between two
network hosts (minimal one-way delay).  This information is then used to
calculate the network trip time for every packet that arrives:

Let DD = Distance from the current packet timestamp difference and minimal.
DD = (Packet Receive - Packet Send) - Min(Packet Receive - Packet Send)
Packet trip time = (Minimal one-way delay) + DD.
