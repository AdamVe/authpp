#pragma once

#include <cstddef>
#include <string>

namespace authpp {

class ByteArray;

class Message {
public:
    Message(std::byte type, ByteArray&& data);
    ~Message();

    std::string toString() const;
    std::byte* get() const;
    std::size_t size() const;

private:
    const std::size_t data_size;
    const std::size_t message_size;
    std::byte* message_buffer;
};

} // namespace authpp
