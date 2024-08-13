#ifndef WAVEMANAGER_HPP
#define WAVEMANAGER_HPP

#include "Entity.hpp"
#include "Spawner.hpp"
#include "Vector.hpp"
#include <chrono>
#include <memory>
#include <vector>

class WaveManager {
public:
  WaveManager(void);
  ~WaveManager(void);

  void setRunAll(bool running);
  void setDelayAll(std::chrono::high_resolution_clock::duration delay);
  void process(void);
  int getWave(void) const;
  void newWave(int n);
  bool allSpwanersEmpty(void) const;
  void nextWave(void);
  template <typename T> void addSpawner(EntityManager<T> &em, Vector pos) {
    _spawners.push_back(std::make_unique<Spawner<T>>(em, pos));
  }

private:
  std::vector<std::unique_ptr<ISpawner>> _spawners;
  int _wave;
  int _mobPerWave;
  bool _allEmpty;
  std::chrono::high_resolution_clock::time_point _waveStart;
  std::chrono::high_resolution_clock::duration _waveDelay;
};

#endif
