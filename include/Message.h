#pragma once

#include <cstddef>
#include <string>

namespace authpp {

class Message {
public:
  Message(std::byte type, std::byte *data, std::size_t dataSize);
  ~Message();

  std::string toString() const;
  std::byte *get() const;
  std::size_t size() const;

private:
  std::size_t messageSize;
  std::byte *messageData;
};

} // namespace authpp
