#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "db/db_connection.h"
#include "net/server.h"

int main(int argc, char *argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: Server <port> [<port> ...]\n";
      return 1;
    }

    std::string configName{"server.json"};
    Db::DbConnParams dbConnParams;
    dbConnParams.dbHost = "localhost";
    dbConnParams.dbPort = 5432;
    dbConnParams.dbName = "otus_messendger";
    dbConnParams.dbUser = "postgres";
    dbConnParams.dbPass = "postgres";

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
