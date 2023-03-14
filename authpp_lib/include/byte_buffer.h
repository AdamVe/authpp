#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <bit>
#include <initializer_list>
#include <vector>

namespace authpp {

class ByteBuffer {
public:
    ByteBuffer();

    explicit ByteBuffer(std::size_t size);

    explicit ByteBuffer(std::initializer_list<uint8_t> il);

    std::size_t size() const;

    void setSize(std::size_t size);

    const ByteBuffer& pointTo(std::size_t index);

    ByteBuffer& setByteOrder(std::endian);

    template <typename T>
    ByteBuffer& put(const T& value)
    {
        assert(pointer < data.size() - sizeof(T) + 1);
        std::memcpy(data.data() + pointer, &value, sizeof(T));
        pointer += sizeof(T);
        return *this;
    }

    template <typename T>
    T get(std::size_t i) const
    {
        assert(i < data.size() - sizeof(T) + 1);
        T value;
        std::memcpy(&value, data.data() + i, sizeof(T));
        return value;
    }

    ByteBuffer get(std::size_t index, std::size_t size) const;

    uint8_t* array() const;

    bool operator==(const ByteBuffer& other) const;

    static void setDebugLog(bool);

private:
    std::endian byteOrder { std::endian::big };
    std::size_t pointer = 0;
    std::vector<uint8_t> data;

    static inline bool debugLog { false };
};

} // namespace authpp
