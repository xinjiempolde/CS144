#include "tcp_receiver.hh"
#include <iostream>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    size_t lastUnassem = _reassembler.stream_out().bytes_written();

    if (seg.header().syn) {
        _isn_flag = true;
        _isn = seg.header().seqno;
        _ackno = _isn;
    }

    cout << seg.header().to_string() << endl;
    uint64_t abseqno = unwrap(seg.header().seqno, _isn, checkpoint); // absolute sequence number
    if (abseqno == 0) {
        abseqno += 1;
    }
    checkpoint = abseqno; 
    if ( _isn_flag && seg.header().seqno.raw_value() + seg.payload().copy().size() >= _ackno.raw_value()) {
        _reassembler.push_substring(seg.payload().copy(), abseqno - 1, seg.header().fin);
    }
    // cout << "data is:" << seg.payload().copy() << endl << "absolute sequence number is: " << abseqno << endl;
    size_t curUnassem = _reassembler.stream_out().bytes_written();
    if (seg.header().syn) {
        _ackno = seg.header().seqno + seg.length_in_sequence_space();
    } else {
        _ackno = _ackno + (curUnassem - lastUnassem);
        if (_reassembler.stream_out().input_ended()) {
            _ackno = _ackno + 1;
        }
    }

}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_isn_flag) {
        return nullopt;
    } else {
        return _ackno;
    }
}

size_t TCPReceiver::window_size() const {
    // window size should be bytes having not been reading(but been reassembled) by application.
    return _capacity - (_reassembler.stream_out().bytes_written() - _reassembler.stream_out().bytes_read());
}
