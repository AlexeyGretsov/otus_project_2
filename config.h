#pragma once

#include <string>
#include <string_view>

#include "db/db_connection.h"

class Config {
public:
  Config(std::string_view fileName);

  bool load();
  Db::DbConnParams getDbParams() const;

private:
  std::string fileName;

  Db::DbConnParams dbConnParams;
};
