#include <csignal>
#include <iostream>
#include <stdexcept>

#include "Server.hpp"

int main() {
  try {
    Server server;
    g_serverInstance = &server;
    std::signal(SIGINT, signalHandler);
    server.init();
    server.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
