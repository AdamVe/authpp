#pragma once

#include <cstddef>
#include <string>

namespace authpp {

class ByteArray;

class Message {
 public:
  Message(std::byte type, ByteArray&& data);
  ~Message();

  std::byte* Get() const;
  std::size_t Size() const;

 private:
  const std::size_t data_size;
  const std::size_t message_size;
  std::byte* message_buffer;
};

}  // namespace authpp
