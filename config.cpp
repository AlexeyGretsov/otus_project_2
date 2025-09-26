#include "config.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

Config::Config(std::string_view fileName) : fileName(fileName) {}

bool Config::load() {
  std::ifstream in(fileName);

  if (not in.is_open()) {
    std::cerr << "Failed to open config file" << fileName << std::endl;

    return false;
  }

  std::string source((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

  if (source.empty()) {
    std::cerr << "Empty config file\n";

    return false;
  }

  std::cout << "Read config file: " << source << std::endl;

  nlohmann::json parsed = nlohmann::json::parse(source);

  dbConnParams.clear();
  dbConnParams.dbHost = std::string(parsed["config"]["db"]["dbHost"]);
  dbConnParams.dbPort =
      std::stoi(std::string(parsed["config"]["db"]["dbPort"]));
  dbConnParams.dbName = std::string(parsed["config"]["db"]["dbName"]);
  dbConnParams.dbUser = std::string(parsed["config"]["db"]["dbUser"]);
  dbConnParams.dbPass = std::string(parsed["config"]["db"]["dbPass"]);

  return true;
}

Db::DbConnParams Config::getDbParams() const { return dbConnParams; }