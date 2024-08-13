#include "WaveManager.hpp"
#include "Spawner.hpp"
#include <chrono>

WaveManager::WaveManager(void)
    : _wave(0), _allEmpty(true), _mobPerWave(9),
      _waveDelay(std::chrono::milliseconds(10000)) {}

WaveManager::~WaveManager(void) {}

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
  auto currentTime = std::chrono::high_resolution_clock::now();
  if (currentTime - _waveStart < _waveDelay)
    return;
  for (auto &spawner : _spawners)
    spawner->spawnIfNeeded();
}

int WaveManager::getWave(void) const { return _wave; }

void WaveManager::newWave(int n) {
  if (_spawners.empty())
    return;
  _waveStart = std::chrono::high_resolution_clock::now();
  _wave++;
  int div = n / _spawners.size();
  int remain = n % _spawners.size();
  for (auto &spawner : _spawners)
    spawner->addToSpawn(div);
  _spawners[0]->addToSpawn(remain);
}

bool WaveManager::allSpwanersEmpty(void) const {
  for (auto &spawner : _spawners)
    if (spawner->leftToSpawn() > 0)
      return false;
  return true;
}

void WaveManager::nextWave(void) { newWave(_mobPerWave + 3); }
