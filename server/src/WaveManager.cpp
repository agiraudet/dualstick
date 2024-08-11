#include "WaveManager.hpp"
#include "Spawner.hpp"

WaveManager::WaveManager(void) {}

WaveManager::~WaveManager(void) {}

WaveManager &WaveManager::WM(void) {
  static WaveManager instance;
  return instance;
}

void WaveManager::setRunAll(bool running) {
  for (auto &spawner : _spawners)
    spawner->setRun(running);
}

void WaveManager::setDelayAll(
    std::chrono::high_resolution_clock::duration delay) {
  for (auto &spawner : _spawners)
    spawner->setDelay(delay);
}

void WaveManager::process() {
  for (auto &spawner : _spawners)
    spawner->spawnIfNeeded();
}

void WaveManager::newWave(int n) {
  if (_spawners.empty())
    return;
  int div = n / _spawners.size();
  int remain = n % _spawners.size();
  for (auto &spawner : _spawners)
    spawner->addToSpawn(div);
  _spawners[0]->addToSpawn(remain);
}
