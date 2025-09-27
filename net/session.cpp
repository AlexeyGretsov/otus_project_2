#include "net/session.h"

#include <boost/uuid/uuid_io.hpp>

#include <iostream>

#include "db/db_manager.h"
#include "include/message.h"

Session::Session(tcp::socket socket, SessionsManager &sessionManager)
    : socket(std::move(socket)), sessionManager(sessionManager) {}

void Session::start(const Db::DbConnParams &params) {
  sessionManager.addSession(shared_from_this());

  dbConnectionPtr = createDbConnection(params);
  if (not dbConnectionPtr) {
    std::cerr << "Failed to start session: no DB connection" << std::endl;

    return;
  }
  std::cout << "Session starting" << std::endl;
  doReadHeader();
}

void Session::doReadHeader() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket,
      boost::asio::buffer(&readMessage.getSizeBuffer(),
                          TransferMessageV2::HEADER_SIZE),
      [this, self](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
          if (readMessage.getLength()) {
            doReadBody();
          }
        } else {
          std::cout << "Client: " << boost::uuids::to_string(clientUuid)
                    << " disconnected" << std::endl;
          sessionManager.removeSession(shared_from_this());
        }
      });
}

void Session::doReadBody() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket,
      boost::asio::buffer(readMessage.getBodyBuffer(), readMessage.getLength()),
      [this, self](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
          saveMessage(readMessage);

          doReadHeader();
        } else {
          std::cout << "Client: " << clientUuid << " disconnected" << std::endl;
          sessionManager.removeSession(shared_from_this());
        }
      });
}

void Session::doWrite() {
  auto self(shared_from_this());
  boost::asio::async_write(
      socket, writeMessages.front().as_buffers(),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          auto sentMsg = writeMessages.front();
          std::cout << "Sent message" << sentMsg.getBody() << std::endl;

          if (not clearProcessedMessage(sentMsg)) {
            std::runtime_error("Failed to clear processed message");
          }

          writeMessages.pop_front();
          if (!writeMessages.empty()) {
            doWrite();
          } else {
            sessionManager.checkProcessedMessages();
          }
        } else {
          std::cout << "Client: " << clientUuid << " disconnected" << std::endl;
          sessionManager.removeSession(shared_from_this());
        }
      });
}

bool Session::saveMessage(const TransferMessageV2 &readMessage) {
  Message msg;
  if (not msg.fromJson(readMessage.getBody())) {
    std::cerr << "Failed to parse received message" << std::endl;

    return false;
  }
  if (msg.isAuth()) {
    std::cerr << "Received auth message " << msg.toJson() << std::endl;

    clientUuid = msg.from;

    sessionManager.checkProcessedMessages();

    return true;
  }

  auto save = [](DbConnectionPtr dbConnectionPtr, const Message *msg) -> bool {
    // TODO think about DB transaction ...

    DbManager dbManager(dbConnectionPtr);
    if (not dbManager.saveMessage(*msg)) {
      std::cerr << "Failed to save message to DB" << std::endl;

      return false;
    }
    if (not dbManager.saveProcessedMessage(msg->to, msg->id)) {
      std::cerr << "Failed to save processed message to DB" << std::endl;
      dbManager.deleteMessage(msg->id);

      return false;
    }

    return true;
  };

  if (not save(dbConnectionPtr, &msg)) {
    return false;
  }

  std::cerr << "Received message " << msg.toJson() << std::endl;

  // Message to send back with status 'processed'
  StatusMessage statusMsg(msg.from, msg.id, Message::STATUS_PROCESSED);

  if (not save(dbConnectionPtr, &statusMsg)) {
    return false;
  }

  sessionManager.checkProcessedMessages();

  return true;
}

void Session::checkProcessedMessages() {
  if (clientUuid.is_nil()) {
    /*
    Connection already created by still not authorized
    */
    std::cerr << "No client UUID after authorization" << std::endl;
    return;
  }

  if (writeMessages.empty()) {
    DbManager dbManager(dbConnectionPtr);
    std::vector<boost::uuids::uuid> ids =
        dbManager.loadProcessedMessages(clientUuid);

    for (auto id : ids) {
      Message msg = dbManager.loadMessage(id);
      if (msg.isValid()) {
        std::string s = msg.toJson();

        TransferMessageV2 transferMsg{msg.toJson()};

        writeMessages.push_back(transferMsg);
      }
    }

    if (not writeMessages.empty()) {

      doWrite();
    }
  }
}

bool Session::clearProcessedMessage(const TransferMessageV2 &transferMessage) {
  Message msg;
  msg.fromJson(transferMessage.getBody());

  DbManager dbManager(dbConnectionPtr);
  return dbManager.deleteProcessedMessage(msg.id);
}