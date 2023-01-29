#pragma once

#include <cstddef>
#include <cstring>

namespace authpp {

class ByteArray {
public:
    explicit ByteArray(std::size_t n)
        : n(n)
        , dn(n)
        , buf((std::byte*)::operator new(n))
    {
    }

    template <std::size_t N>
    explicit ByteArray(const unsigned char (&a)[N])
        : n(N)
        , dn(N)
        , buf((std::byte*)::operator new(N))
    {
        std::memcpy(buf, a, N);
    }

    ByteArray(const ByteArray& other)
        : n(other.n)
        , dn(other.dn)
        , buf((std::byte*)::operator new(n))
    {
        std::memcpy(buf, other.buf, n);
    }

    ByteArray(ByteArray&& other)
        : n(other.n)
        , dn(other.dn)
        , buf(other.buf)
    {
        other.buf = nullptr;
        other.dn = 0;
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

    std::size_t getDataSize() const
    {
        return dn;
    }

    void setDataSize(std::size_t n)
    {
        dn = n;
    }

private:
    std::size_t n;
    std::size_t dn;
    std::byte* buf;
};

} // namespace authpp
