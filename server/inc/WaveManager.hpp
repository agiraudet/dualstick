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
  static WaveManager &WM(void);

  WaveManager(WaveManager const &) = delete;
  void operator=(WaveManager const &) = delete;

  void setRunAll(bool running);
  void setDelayAll(std::chrono::high_resolution_clock::duration delay);
  void process(void);
  int getWave(void) const;
  void newWave(int n);
  template <typename T> void addSpawner(EntityManager<T> &em, Vector pos) {
    _spawners.push_back(std::make_unique<Spawner<T>>(em, pos));
  }

private:
  WaveManager(void);
  ~WaveManager(void);

private:
  std::vector<std::unique_ptr<ISpawner>> _spawners;
  int _wave;
};

#endif
