#include <iostream>
#include <stdexcept>

#include "Server.hpp"

int main() {
  try {
    Server server;
    server.init();
    server.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
