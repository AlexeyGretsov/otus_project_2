#pragma once

#include <deque>
#include <iostream>

#include <boost/asio.hpp>

#include "include/message.h"
#include "include/transfer_message.h"

using boost::asio::ip::tcp;

typedef std::deque<TransferMessage> TransferMessageQueue;

class Client {
public:
  Client(boost::asio::io_context &io_context,
         const tcp::resolver::results_type &endpoints);

  void write(const TransferMessage &msg);
  void close();

private:
  void doConnect(const tcp::resolver::results_type &endpoints);
  void doReadHeader();
  void doReadBody();
  void doWrite();

private:
  bool processMessage(const TransferMessage &readMessage);

  boost::asio::io_context &io_context;
  tcp::socket socket;
  TransferMessage readMessage;
  TransferMessageQueue writeMessages;
};
