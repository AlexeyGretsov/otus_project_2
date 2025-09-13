#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>

#include "db/db_connection.h"
#include "include/i_messages_processor.h"
#include "net/session_manager.h"

using boost::asio::ip::tcp;

class Session : public MessagesProcessor,
                public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket, SessionsManager &sessionManager);
  void start(const Db::DbConnParams &params);

private:
  void doReadHeader();
  void doReadBody();
  void doWrite();
  bool saveMessage(const TransferMessage &readMessage);
  void checkProcessedMessages() override;
  bool clearProcessedMessage(const TransferMessage &transferMessage);

  boost::uuids::uuid clientUuid;
  tcp::socket socket;
  TransferMessage readMessage;
  TransferMessageQueue writeMessages;
  DbConnectionPtr dbConnectionPtr;
  SessionsManager &sessionManager;
};
