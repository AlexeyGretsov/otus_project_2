#pragma once

#include <deque>
#include <iostream>

#include <boost/asio.hpp>

#include "include/message.h"
#include "include/transfer_message_v2.h"

using boost::asio::ip::tcp;

typedef std::deque<TransferMessageV2> TransferMessageQueue;

class Client {
public:
  Client(boost::asio::io_context &io_context,
         const tcp::resolver::results_type &endpoints);

  void write(const TransferMessageV2 &msg);
  void close();

private:
  void doConnect(const tcp::resolver::results_type &endpoints);
  void doReadHeader();
  void doReadBody();
  void doWrite();

private:
  bool processMessage(const TransferMessageV2 &readMessage);

  boost::asio::io_context &io_context;
  tcp::socket socket;
  TransferMessageV2 readMessage;
  TransferMessageQueue writeMessages;
};
