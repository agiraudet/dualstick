#include "Mob.hpp"
#include <vector>

int Mob::count = 0;

Mob::Mob(void) : Entity(count++) {}

void Mob::setType(mobType type) { _type = type; }

mobType Mob::getType(void) const { return _type; }

void Mob::findClosest(std::vector<Player *> &playerVec) {
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
  }
}
