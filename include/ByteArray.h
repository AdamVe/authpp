#pragma once

#include <cstddef>
#include <cstring>

namespace authpp {

class ByteArray {
public:
    ByteArray(std::size_t n)
        : n(n)
        , buf((std::byte*)::operator new(n))
    {
    }

    template <std::size_t N>
    ByteArray(const unsigned char (&a)[N])
        : n(N)
        , buf((std::byte*)::operator new(N))
    {
        std::memcpy(buf, a, N);
    }

    ByteArray(const ByteArray& other)
        : n(other.n)
        , buf((std::byte*)::operator new(n))
    {
        std::memcpy(buf, other.buf, n);
    }

    ByteArray(ByteArray&& other)
        : n(other.n)
        , buf(other.buf)
    {
        other.buf = nullptr;
        other.n = 0;
    }

    ~ByteArray()
    {
        ::operator delete(buf);
    }

    std::byte* get() const
    {
        return buf;
    }

    std::size_t getSize() const
    {
        return n;
    }

private:
    std::size_t n;
    std::byte* buf;
};

} // namespace authpp
