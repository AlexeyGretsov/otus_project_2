#include "net/server.h"

Server::Server(boost::asio::io_context &io_context,
               const tcp::endpoint &endpoint, const Db::DbConnParams &params)
    : acceptor(io_context, endpoint), params(params) {
  doAccept();
}

void Server::doAccept() {
  acceptor.async_accept(
      [this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
          std::cout << "Accept conection" << std::endl;
          std::make_shared<Session>(std::move(socket), sessionManager)
              ->start(params);
        }

        doAccept();
      });
}
