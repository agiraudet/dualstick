#include "Engine.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  int engCo;
  Engine eng;
  if (argc >= 3)
    engCo = eng.connect(argv[1], atoi(argv[2]));
  else if (argc >= 2)
    engCo = eng.connect(argv[1]);
  else
    engCo = eng.connect();
  if (engCo != 0) {
    std::cerr << "Failed to connect to the server !" << std::endl;
    return EXIT_FAILURE;
  }
  eng.run();
  return EXIT_SUCCESS;
}
