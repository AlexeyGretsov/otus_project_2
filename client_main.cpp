#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include <boost/uuid/string_generator.hpp>

#include "net/client.h"

using boost::asio::ip::tcp;

inline void usage() {
  std::cerr << "Usage: Client <host> <port> <my UUID> <remote UUID>\n";
}

inline void authorize(Client &c, boost::uuids::uuid &from) {
  TransferMessage transferMessage(AuthMessage(from).toJson());
  c.write(transferMessage);
}

int main(int argc, char *argv[]) {
  try {
    if (argc != 5) {
      usage();
      return EXIT_FAILURE;
    }

    boost::uuids::string_generator sgen;
    boost::uuids::uuid from = sgen(argv[3]);
    boost::uuids::uuid to = sgen(argv[4]);

    if (from.is_nil()) {
      std::cerr << "Invalid parameter: my UUID" << std::endl;
      usage();
      return EXIT_FAILURE;
    }

    if (to.is_nil()) {
      std::cerr << "Invalid parameter: remote UUID" << std::endl;
      usage();
      return EXIT_FAILURE;
    }

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    Client c(io_context, endpoints);

    std::thread t([&io_context]() { io_context.run(); });

    authorize(c, from);

    char line[TransferMessage::MAX_BODY_LENGTH + 1];

    while (std::cin.getline(line, TransferMessage::MAX_BODY_LENGTH + 1)) {
      if (std::strlen(line) == 0) {
        continue;
      }
      TextMessage msg(from, to, line);

      c.write(TransferMessage(msg.toJson()));
    }

    c.close();
    t.join();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
