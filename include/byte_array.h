#pragma once

#include <cstddef>
#include <cstring>

namespace authpp {

class XByteArray {
public:
    XByteArray() { }

    explicit XByteArray(std::size_t n)
        : n(n)
        , dn(n)
        , buf((std::byte*)::operator new(n))
    {
    }

    template <std::size_t N>
    explicit XByteArray(const unsigned char (&a)[N])
        : n(N)
        , dn(N)
        , buf((std::byte*)::operator new(N))
    {
        std::memcpy(buf, a, N);
    }

    XByteArray(const XByteArray& other)
        : n(other.n)
        , dn(other.dn)
        , buf((std::byte*)::operator new(n))
    {
        std::memcpy(buf, other.buf, n);
    }

    XByteArray(const XByteArray& other, const std::size_t& offset,
        const std::size_t& length)
        : n(length)
        , dn(length)
        , buf((std::byte*)::operator new(n))
    {
        std::memcpy(buf, other.buf + offset, length);
    }

    XByteArray(XByteArray&& other)
        : n(other.n)
        , dn(other.dn)
        , buf(other.buf)
    {
        other.buf = nullptr;
        other.dn = 0;
        other.n = 0;
    }

    ~XByteArray() { ::operator delete(buf); }

    XByteArray& operator=(const XByteArray& other)
    {
        n = other.n;
        dn = other.dn;
        delete[] buf;
        buf = (std::byte*)::operator new(n);
        std::memcpy(buf, other.buf, n);
        return *this;
    }

    std::byte* Get() const { return buf; }

    std::byte Get(const std::size_t& i) const { return buf[i]; }

    std::size_t GetDataSize() const { return dn; }

    void SetDataSize(std::size_t n) { dn = n; }

private:
    std::size_t n = 0;
    std::size_t dn = 0;
    std::byte* buf = nullptr;
};

} // namespace authpp
