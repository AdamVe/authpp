#pragma once

#include <bytes.h>

#include <cstddef>
#include <string>

namespace authpp {

class Message {
public:
    Message(uint8_t type, const Bytes& data);
    ~Message();

    const Bytes& get() const;

private:
    Bytes bytes;
};

} // namespace authpp
