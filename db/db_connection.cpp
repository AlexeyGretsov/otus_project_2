#include "db/db_connection.h"

namespace Db {
void DbConnParams::clear() {
  dbHost.clear();
  dbPort = 0;
  ;
  dbName.clear();
  dbUser.clear();
  dbPass.clear();
}

DbConnectionPtr createDbConnection(const DbConnParams &params) {
  if (params.dbHost.empty()) {
    std::cerr << "Invalid DB host";

    return nullptr;
  }
  if (params.dbPort <= 0) {
    std::cerr << "Invalid DB port";

    return nullptr;
  }
  if (params.dbName.empty()) {
    std::cerr << "Invalid DB name";

    return nullptr;
  }
  if (params.dbUser.empty()) {
    std::cerr << "Invalid DB user";

    return nullptr;
  }

  DbConnectionPtr dbConn(new Db::PgDbConnection());
  if (not dbConn->connect(params.dbHost, params.dbPort, params.dbName,
                          params.dbUser, params.dbPass)) {
    std::cerr << "Failed to connect DB" << std::endl;
    return nullptr;
  }

  return dbConn;
}
} // namespace Db
