#include "byte_stream.hh"
#include <iostream>
// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capa) {
    this->capacity = capa;
    this->used = 0;
    this->total_read = 0;
    this->total_write = 0;
    this->ended = false;
 }

size_t ByteStream::write(const string &data) {
    if (input_ended()) {
        return 0;
    }
    int write_bytes = 0;
    if (data.size() > capacity - used) { // cannot store whole data
        write_bytes = capacity - used;
    } else {
        write_bytes = data.size();
    }
    for (int i = 0; i < write_bytes; i++) {
        buffer.push_back(data[i]);
    }
    used += write_bytes;
    total_write += write_bytes;
    cout << "written bytes were " << write_bytes << endl;
    return write_bytes;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    int read_size = len > used ? used : len;
    string ret = "";
    for (int i = 0; i < read_size; i++) {
        ret += buffer[i];
    }
    return ret;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    int real_size = len > used ? used : len;
    auto begin = buffer.begin();
    auto end = begin + real_size;
    buffer.erase(begin, end);
    used -= real_size;
    total_read += real_size;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string ret = peek_output(len);
    pop_output(len);
    return ret;
}

void ByteStream::end_input() {
    ended = true;
}

bool ByteStream::input_ended() const {
    return ended;
}

size_t ByteStream::buffer_size() const {
    return used;
}

bool ByteStream::buffer_empty() const {
    return used == 0 ? true : false;
}

bool ByteStream::eof() const { 
    return ended && buffer_empty(); 
}

size_t ByteStream::bytes_written() const {
    return total_write;
}

size_t ByteStream::bytes_read() const {
    return total_read;
}

size_t ByteStream::remaining_capacity() const {
    return capacity - used;
}
