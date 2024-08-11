#ifndef SPAWNER_HPP
#define SPAWNER_HPP

#include "Entity.hpp"
#include "Vector.hpp"
#include <chrono>

class ISpawner {
public:
  ISpawner(void);
  virtual ~ISpawner(void) = default;
  virtual void spawnIfNeeded(void) = 0;

public:
  void setPos(Vector pos);
  void setRun(bool running);
  void setDelay(std::chrono::high_resolution_clock::duration delay);
  void addToSpawn(int n);

protected:
  bool _running;
  Vector _position;
  int _toSpawn;
  std::chrono::high_resolution_clock::time_point _lastSpawn;
  std::chrono::high_resolution_clock::duration _delay;
};

template <typename T> class Spawner : public ISpawner {
public:
  Spawner(EntityManager<T> &em) : _em(em) {
    _lastSpawn = std::chrono::high_resolution_clock::now();
  }

  Spawner(EntityManager<T> &em, Vector pos) : _em(em) {
    _position = pos;
    _lastSpawn = std::chrono::high_resolution_clock::now();
  }

  ~Spawner(void) {}

  void spawnIfNeeded(void) {
    if (!_running || _toSpawn <= 0)
      return;
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (currentTime - _lastSpawn < _delay)
      return;
    _lastSpawn = currentTime;
    _toSpawn--;
    _em.get(_em.create())->setPos(_position);
  }

private:
  EntityManager<T> &_em;
};

#endif
