#include "Spawner.hpp"
#include "Vector.hpp"

ISpawner::ISpawner(void)
    : _running(false), _position(0, 0), _toSpawn(0), _delay(0) {}

void ISpawner::setPos(Vector pos) { _position = pos; }

void ISpawner::setRun(bool running) { _running = running; }

void ISpawner::setDelay(std::chrono::high_resolution_clock::duration delay) {
  _delay = delay;
}

void ISpawner::addToSpawn(int n) { _toSpawn += n; }
