#include "Mob.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include <chrono>
#include <memory>

Mob::Mob(void) : Entity() {
  _maxSpeed *= 0.5;
  weapon = new Weapon;
  _initWeapon();
}

Mob::Mob(int id) : Entity() {
  _id = id;
  _maxSpeed *= 0.5;
  weapon = new Weapon;
  _initWeapon();
}

Mob::~Mob(void) {}

Vector const &Mob::getTarget(void) const { return _target; }

std::shared_ptr<Player>
Mob::findClosest(EntityManager<Player> const &EMPlayer) {
  double minDist = 0.f;
  std::shared_ptr<Player> ptr = nullptr;
  for (auto it = EMPlayer.begin(); it != EMPlayer.end(); it++) {
    double dist = _position.distanceSq(it->second->getPos());
    if (!ptr || dist < minDist) {
      minDist = dist;
      ptr = it->second;
    }
  }
  if (ptr) {
    _target = ptr->getPos();
    aimAngle(_target.x, _target.y);
  }
  return ptr;
}

void Mob::_initWeapon(void) {
  if (weapon) {
    weapon->unlimitedAmmo = true;
    weapon->range = 40.f;
    weapon->damage = 1;
    weapon->cd = std::chrono::milliseconds(1000);
  }
}
