#include "Mob.hpp"
#include "Weapon.hpp"
#include <chrono>
#include <vector>

int Mob::count = 0;

Mob::Mob(void) : Entity(count++) {
  _maxSpeed *= 0.5;
  weapon = new Weapon;
  weapon->unlimitedAmmo = true;
  weapon->range = 40.f;
  weapon->damage = 1;
  weapon->cd = std::chrono::milliseconds(1000);
}

void Mob::setType(mobType type) { _type = type; }

mobType Mob::getType(void) const { return _type; }

Vector const &Mob::getTarget(void) const { return _target; }

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
