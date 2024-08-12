#include "Mob.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include <chrono>
#include <memory>

Mob::Mob(void) : Entity() {
  _maxSpeed *= 0.2;
  weapon = new Weapon;
  _initWeapon();
  _target = nullptr;
}

Mob::Mob(int id) : Entity() {
  _id = id;
  _maxSpeed *= 0.2;
  weapon = new Weapon;
  _initWeapon();
  _target = nullptr;
}

Mob::~Mob(void) {}

std::shared_ptr<Player> Mob::getTarget(void) const { return _target; }

std::shared_ptr<Player>
Mob::findClosest(EntityManager<Player> const &EMPlayer) {
  double minDist = 0.f;
  std::shared_ptr<Player> ptr = nullptr;
  for (auto it = EMPlayer.begin(); it != EMPlayer.end(); it++) {
    if (!it->second->isAlive())
      continue;
    double dist = _position.distanceSq(it->second->getPos());
    if (!ptr || dist < minDist) {
      minDist = dist;
      ptr = it->second;
    }
  }
  _target = ptr;
  if (ptr)
    aimAngle(_target->getPos().x, _target->getPos().y);
  return _target;
}

void Mob::_initWeapon(void) {
  if (weapon) {
    weapon->unlimitedAmmo = true;
    weapon->range = 40.f;
    weapon->damage = 1;
    weapon->cd = std::chrono::milliseconds(1000);
  }
}

void Mob::processDir(Vector aim, int tileSize) {
  if (aim.x == 0.f && aim.y == 0.f) {
    setVel(aim);
    return;
  }
  aim.x = (getTileX() + aim.x) * tileSize + (float)tileSize / 2;
  aim.y = (getTileY() + aim.y) * tileSize + (float)tileSize / 2;
  aim -= _position;
  aim.normalize();
  aim = aim * _maxSpeed;
  // TODO we should check that the vector is never longer that a tile;
  setVel(aim);
}

bool Mob::tryHitTarget(void) {
  if (!_target || !weapon)
    return false;
  if (_position.distance(_target->getPos()) <= weapon->range) {
    if (weapon->fire()) {
      weapon->dealDamage(*_target, 0.f);
      return true;
    }
  }
  return false;
}

void Mob::setVelTowards(Vector target) {
  Vector dir = target - _position;
  dir.normalize();
  dir = dir * _maxSpeed;
  setVel(dir);
}
