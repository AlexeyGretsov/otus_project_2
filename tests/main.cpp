#include <cstdlib>

#include "tests/db_manager_test.h"
#include "tests/db_test.h"
#include "tests/json_test.h"
#include "tests/message_test.h"

int main(int argc, char *argv[]) {
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
}
