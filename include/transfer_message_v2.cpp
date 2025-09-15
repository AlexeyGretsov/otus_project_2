#include "include/transfer_message_v2.h"

#include <iostream>

// #include <boost/asio.hpp>
// #include <boost/endian.hpp>

TransferMessageV2::TransferMessageV2() {
  size = 0;
  std::fill(body.begin(), body.end(), 0);
}
TransferMessageV2::TransferMessageV2(const std::string &source) {
  std::fill(body.begin(), body.end(), 0);
  std::copy(std::begin(source), std::end(source), std::begin(body));
  size = source.size();
}
TransferMessageV2::~TransferMessageV2() {}

TransferMessageV2::TransferMessageV2(const TransferMessageV2 &other) {
  size = other.size;
  std::fill(body.begin(), body.end(), 0);
  std::copy(std::begin(other.body), std::end(other.body), std::begin(body));
}
TransferMessageV2::TransferMessageV2(TransferMessageV2 &&other) {
  size = other.size;
  body = std::move(other.body);
}
TransferMessageV2 &
TransferMessageV2::operator=(const TransferMessageV2 &other) {
  if (this != &other) {
    size = other.size;
    std::fill(body.begin(), body.end(), 0);
    std::copy(std::begin(other.body), std::end(other.body), std::begin(body));
  }

  return *this;
}
TransferMessageV2 &TransferMessageV2::operator=(TransferMessageV2 &&other) {
  if (this != &other) {
    size = other.size;
    body = std::move(other.body);
  }
  return *this;
}

uint32_t TransferMessageV2::getLength() const { return size; }
uint32_t &TransferMessageV2::getSizeBuffer() { return size; }
std::array<char, TransferMessageV2::MAX_MESSAGE_SIZE> &
TransferMessageV2::getBodyBuffer() {
  return body;
}
std::string TransferMessageV2::getBody() const {
  return std::string(std::begin(body), size);
}

std::array<boost::asio::const_buffer, 2> TransferMessageV2::as_buffers() {
  return {boost::asio::buffer(&size, sizeof(size)),
          boost::asio::buffer(body, size)};
}
