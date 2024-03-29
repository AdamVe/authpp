#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <bit>
#include <initializer_list>
#include <vector>

namespace authpp {

template<std::integral T>
T byteswap(T b);

class ByteBuffer {
public:
    ByteBuffer();

    explicit ByteBuffer(std::size_t size);

    ByteBuffer(std::initializer_list<uint8_t> il);

    [[nodiscard]] std::size_t size() const;

    void setSize(std::size_t size);

    const ByteBuffer& pointTo(std::size_t index);

    ByteBuffer& setByteOrder(std::endian);

    ByteBuffer& put(const ByteBuffer& buffer);

    ByteBuffer& put(const std::integral auto& value)
    {
        assert(pointer < data.size() - sizeof(value) + 1);
        if (std::endian::native == byteOrder) {
            std::memcpy(data.data() + pointer, &value, sizeof(value));
        } else {
            auto swapped = byteswap(value);
            std::memcpy(data.data() + pointer, &swapped, sizeof(swapped));
        }
        pointer += sizeof(value);
        return *this;
    }

    template <std::integral T>
    T get(std::size_t i) const
    {
        assert(i < data.size() - sizeof(T) + 1);
        T value;
        std::memcpy(&value, data.data() + i, sizeof(T));
        if (std::endian::native != byteOrder) {
            value = byteswap(value);
        }
        return value;
    }

    [[nodiscard]] ByteBuffer get(std::size_t index, std::size_t size) const;

    [[nodiscard]] uint8_t* array() const;

    bool operator==(const ByteBuffer& other) const;

    static void setDebugLog(bool);

private:
    std::endian byteOrder { std::endian::little };
    std::size_t pointer = 0;
    std::vector<uint8_t> data;

    static inline bool debugLog { false };
};

} // authpp
