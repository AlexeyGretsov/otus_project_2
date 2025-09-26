#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "db/pg_db_connection.h"

using DbConnectionPtr = std::shared_ptr<Db::IDbConnection>;

namespace Db {
struct DbConnParams {
  void clear();

  std::string dbHost;
  int dbPort{0};
  std::string dbName;
  std::string dbUser;
  std::string dbPass;
};

DbConnectionPtr createDbConnection(const DbConnParams &params);
} // namespace Db
