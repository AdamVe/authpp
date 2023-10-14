#pragma once

#include "byte_buffer.h"

#include <cstddef>
#include <string>

namespace authpp {

class Message {
public:
    Message(uint8_t type, const ByteBuffer& data);
    ~Message();

    [[nodiscard]] const ByteBuffer& get() const;

private:
    ByteBuffer buffer;
};

} // authpp
