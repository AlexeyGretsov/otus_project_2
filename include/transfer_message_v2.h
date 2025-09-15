#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <boost/asio.hpp>

using header_t = uint32_t;

class TransferMessageV2 {
public:
  static constexpr int MAX_MESSAGE_SIZE = 8192;
  static constexpr int HEADER_SIZE = sizeof(header_t);

  TransferMessageV2();
  TransferMessageV2(const std::string &source);
  ~TransferMessageV2();
  TransferMessageV2(const TransferMessageV2 &other);
  TransferMessageV2(TransferMessageV2 &&other);
  TransferMessageV2 &operator=(const TransferMessageV2 &other);
  TransferMessageV2 &operator=(TransferMessageV2 &&other);

  uint32_t getLength() const;
  uint32_t &getSizeBuffer();
  std::array<char, TransferMessageV2::MAX_MESSAGE_SIZE> &getBodyBuffer();
  std::string getBody() const;

  std::array<boost::asio::const_buffer, 2> as_buffers();

private:
  header_t size{0};
  std::array<char, TransferMessageV2::MAX_MESSAGE_SIZE> body;
};
