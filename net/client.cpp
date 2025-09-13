#include "net/client.h"

Client::Client(boost::asio::io_context &io_context,
               const tcp::resolver::results_type &endpoints)
    : io_context(io_context), socket(io_context) {
  doConnect(endpoints);
}

void Client::write(const TransferMessage &msg) {
  boost::asio::post(io_context, [this, msg]() {
    bool write_in_progress = !writeMessages.empty();

    writeMessages.push_back(msg);
    if (!write_in_progress) {
      doWrite();
    }
  });
}

void Client::close() {
  boost::asio::post(io_context, [this]() { socket.close(); });
}

void Client::doConnect(const tcp::resolver::results_type &endpoints) {
  boost::asio::async_connect(
      socket, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
        if (!ec) {
          std::cout << "Connection extablished" << std::endl;
          doReadHeader();
        }
      });
}

void Client::doReadHeader() {
  boost::asio::async_read(
      socket,
      boost::asio::buffer(readMessage.getData(), TransferMessage::headerLength),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec && readMessage.decodeHeader()) {
          doReadBody();
        } else {
          socket.close();
        }
      });
}

void Client::doReadBody() {
  boost::asio::async_read(
      socket,
      boost::asio::buffer(readMessage.getBody(), readMessage.getBodyLength()),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          if (not processMessage(readMessage)) {
            socket.close();
            return;
          }
          doReadHeader();
        } else {
          socket.close();
        }
      });
}

void Client::doWrite() {
  boost::asio::async_write(
      socket,
      boost::asio::buffer(writeMessages.front().getData(),
                          writeMessages.front().length()),
      [this](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          writeMessages.pop_front();
          if (!writeMessages.empty()) {
            doWrite();
          }
        } else {
          socket.close();
        }
      });
}

bool Client::processMessage(const TransferMessage &readMessage) {
  Message msg;
  if (not msg.fromJson(
          std::string(readMessage.getBody(), readMessage.getBodyLength()))) {
    std::cerr << "Failed to parse received message" << std::endl;

    return false;
  }

  if (msg.isText()) {
    TextMessageJson *json = static_cast<TextMessageJson *>(msg.json.get());
    std::cout << "[Получено сообщение] " << json->text << std::endl;

    StatusMessage statusMessage(msg.from, msg.id, Message::STATUS_RECEIVED);
    TransferMessage transferMessage(statusMessage.toJson());
    write(transferMessage);
  } else if (msg.isStatus()) {
    StatusMessageJson *json = static_cast<StatusMessageJson *>(msg.json.get());
    std::cout << "[Статус сообщения] " << json->status << std::endl;
  } else {
    throw std::runtime_error("Undefined message received");
  }

  return true;
}
