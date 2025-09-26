#pragma once

#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "net/session.h"
#include "net/session_manager.h"

using boost::asio::ip::tcp;

class Server {
public:
  Server(boost::asio::io_context &io_context, const tcp::endpoint &endpoint,
         const Db::DbConnParams &params);

private:
  void doAccept();

  tcp::acceptor acceptor;
  SessionsManager sessionManager;
  Db::DbConnParams params;
};
