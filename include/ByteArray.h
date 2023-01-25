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
    }

    ByteArray(const ByteArray& other)
        : data(new std::byte[other.len])
    {
        std::memcpy(data, other.data, other.len);
    }

    ByteArray(ByteArray&& other)
        : len(other.len)
        , data(other.data)
    {
        other.data = nullptr;
        other.len = 0;
    }

    ~ByteArray()
    {
        if (data != nullptr) {
            delete[] data;
        }
    }

    std::byte* get() const
    {
        return data;
    }

    std::size_t getSize() const
    {
        return len;
    }

private:
    std::size_t len;
    std::byte* data;
};

} // namespace authpp
