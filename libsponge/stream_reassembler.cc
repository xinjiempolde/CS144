#include "stream_reassembler.hh"
#include <iostream>
#include <algorithm>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`


using namespace std;

Record::Record(uint64_t index, const string& data, size_t len):
    _index(index),
    _data(data),
    _len(len) {
}

Record::~Record(){
}

bool comp(const Record &a, const Record &b) {
    return a._index < b._index;
}

StreamReassembler::StreamReassembler(const size_t capacity) : 
    _cache(),
    _expect_index(0),
    _output(capacity), 
    _ended(false),
    _loss(false),
    _capacity(capacity){}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (index > _expect_index) { // out-of-order substring
        if (data.size() != 0) {
            _cache.push_back({index, data, data.size()});
            sort(_cache.begin(), _cache.end(), comp);
        }
    } else if (index + data.size() > _expect_index) {
        string tmp = data.substr(_expect_index - index);
        size_t write_bytes = _output.write(tmp);
        if (write_bytes < tmp.size()) {
            _loss = true;
        }
        // cout << "write: " << tmp << endl;
        _expect_index = _expect_index + write_bytes;
        for (auto iter = _cache.begin(); iter != _cache.end();) {
            if (iter->_index > _expect_index) {
                break;
            }
            if (iter->_index + iter->_len > _expect_index) {
                tmp = iter->_data.substr(_expect_index - iter->_index);
                // cout << "write string: " << tmp << endl;
                write_bytes = _output.write(tmp);
                if (write_bytes < tmp.size()) {
                    _loss = true;
                }
                // cout << "write: " << tmp << endl;;
                _expect_index = _expect_index + write_bytes;
                iter = _cache.erase(iter);
            } else {
                iter = _cache.erase(iter);
            }
        }
    }

    if (eof) {
        _ended = true;
    }

    if (_ended && (_cache.size() == 0) && !_loss) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t total = 0;
    uint64_t next_index = 0;
    for (auto iter = _cache.begin(); iter != _cache.end(); iter++) {
        if (iter->_index > next_index) {
            total += iter->_len;
            next_index = iter->_index + iter->_len;
        } else {
            if (iter->_index + iter->_len > next_index) {
                total += iter->_len - (next_index - iter->_index);
                next_index = iter->_index + iter->_len;
            }
        }
    }
    return total;
}

bool StreamReassembler::empty() const {
    return _ended && (_cache.size() == 0);
}
