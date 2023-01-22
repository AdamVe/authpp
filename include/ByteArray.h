#pragma once

#include <Logger.h>

#include <cstddef>

namespace authpp {

class ByteArray {
public:
    ByteArray(std::size_t len = 0)
    : len(len)
    , data(new std::byte[len])
    {
        Log.v("Build array of len {}", len);
    }

    ByteArray(const ByteArray& other)
    : data(new std::byte[other.len]) {
        Log.v("Copy ctor");
        std::memcpy(data, other.data, other.len);
    }

    ByteArray(ByteArray&& other)
    : len(other.len)
    , data(other.data) {
        Log.v("Move ctor");
        other.data = nullptr;
        other.len = 0;
    }

    ~ByteArray() {
        if (data != nullptr) {
        Log.v("Releasing array of size {}", len);
        delete[] data;
        }
    }

    std::byte* get() const {
        return data;
    }

    std::size_t getSize() const {
        return len;
    }
private:
    std::size_t len;
    std::byte* data;
};

} // namespace authpp
