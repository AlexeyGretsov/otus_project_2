#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "tests/db_manager_test.h"
#include "tests/db_test.h"
#include "tests/json_test.h"
#include "tests/message_test.h"

#include "db/db_connection.h"
#include "net/server.h"

// #define ENABLE_TESTS

int main(int argc, char *argv[]) {

#ifdef ENABLE_TESTS
  if (not Tests::dbTest()) {
    return EXIT_FAILURE;
  }
  if (not Tests::jsonTest()) {
    return EXIT_FAILURE;
  }
  if (not Tests::dbManagerTest()) {
    return EXIT_FAILURE;
  }
  if (not Tests::messageTest()) {
    return EXIT_FAILURE;
  }

  return 0;
#endif

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
