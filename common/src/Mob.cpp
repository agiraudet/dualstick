#include "Mob.hpp"
#include <vector>

int Mob::count = 0;

Mob::Mob(void) : Entity(count++) {}

void Mob::setType(mobType type) { _type = type; }

mobType Mob::getType(void) const { return _type; }

Player *Mob::findClosest(std::vector<Player *> &playerVec) {
  double minDist = 0.f;
  Player *closestPlayer = nullptr;

  for (auto p : playerVec) {
    double dist = _position.distanceSq(p->getPos());
    if (!closestPlayer || dist < minDist) {
      minDist = dist;
      closestPlayer = p;
    }
  }
  if (closestPlayer) {
    _target = closestPlayer->getPos();
    aimAngle(_target.x, _target.y);
    return closestPlayer;
  }
  return nullptr;
}

bool Mob::futurMovAllowed(Map &map, Vector const &vel) {

  auto currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> timeSinceMove =
      currentTime - _lastMove;
  double deltaTime = timeSinceMove.count() / 1000.0;
  Vector futurPos = _position + _velocity * deltaTime;
  return (!map.boxIsColliding(futurPos.x - (float)_size / 2.f,
                              futurPos.y - (float)_size / 2.f, _size, _size));
}
