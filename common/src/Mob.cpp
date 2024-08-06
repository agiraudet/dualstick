#include "Mob.hpp"
#include <vector>

int Mob::count = 0;

Mob::Mob(void) : Entity(count++) { _maxSpeed *= 0.8; }

void Mob::setType(mobType type) { _type = type; }

mobType Mob::getType(void) const { return _type; }

Player const *Mob::findClosest(std::vector<Player const *> &playerVec) {
  double minDist = 0.f;
  Player const *closestPlayer = nullptr;

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
