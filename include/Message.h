#pragma once

#include "Byte.h"

#include <cstddef>
#include <string>

namespace authpp {

class Message {
public:
  Message(byte type, byte *data, std::size_t dataSize);
  ~Message();

  std::string toString() const;
  byte *get() const;
  std::size_t size() const;

private:
  std::size_t messageSize;
  byte *messageData;
};

} // namespace authpp
