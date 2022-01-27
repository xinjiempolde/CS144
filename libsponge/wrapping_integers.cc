#include "wrapping_integers.hh"
#include <iostream>
#include <cmath>
// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    uint64_t sum = isn.raw_value() + n;
    uint32_t low = sum & 0xffffffff;
    return WrappingInt32(low);
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note sequence number may be roundtrip because it's just 32 bit which corresponds to many
//! absolute sequence number.
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint64_t middle = (checkpoint & 0xffffffff00000000ul) + static_cast<uint32_t>(n - isn); // prevent overflowing
    uint64_t bigger = middle + (1ul << 32);
    uint64_t smaller = middle - (1ul << 32);
    uint64_t distance;
    if (middle > checkpoint) {
       distance = middle - checkpoint;
       // smaller is closest to checkpoint
       if (smaller + (1ul << 32) > smaller && smaller + distance >= checkpoint) { // prevent smaller is negtive.
           return smaller;
       } else { // middle is closest to checkpoint
           return middle;
       } 
    } else {
        distance = bigger - checkpoint;
        return (middle + distance >= checkpoint) ? middle : bigger;
    }
}
