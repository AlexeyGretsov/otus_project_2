#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "config.h"
#include "db/db_connection.h"
#include "net/server.h"

#include "include/transfer_message_v2.h"

inline void usage() {
  std::cerr << "Usage: Server <config file name> <port> [<port> ...]\n";
}

int main(int argc, char *argv[]) {

  TransferMessageV2 m;
  try {
    if (argc < 3) {
      std::cerr << "Arguments not found\n";
      usage();
      return EXIT_FAILURE;
    }

    if (not std::strlen(argv[1])) {
      std::cerr << "Empty config file name\n";
      usage();
      return EXIT_FAILURE;
    }
    Config cfg(argv[1]);
    if (not cfg.load()) {
      usage();
      return EXIT_FAILURE;
    }

    auto dbConnParams = cfg.getDbParams();
    std::cout << "DB connection parameters:" << std::endl;
    std::cout << " - host: " << dbConnParams.dbHost << std::endl;
    std::cout << " - port: " << dbConnParams.dbPort << std::endl;
    std::cout << " - name: " << dbConnParams.dbName << std::endl;
    std::cout << " - user: " << dbConnParams.dbUser << std::endl;
    std::cout << " - pass: " << dbConnParams.dbPass << std::endl;

    boost::asio::io_context io_context;

    std::list<Server> servers;
    for (int i = 1; i < argc; ++i) {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint, dbConnParams);
    }

    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
