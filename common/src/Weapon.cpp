#include "Weapon.hpp"
#include <cstdio>

Weapon::Weapon(void)
    : _damage(5), _range(3000), _ammo(100), _clip(10), _maxClip(10), _cd(500),
      _reloadTime(1000) {
  _lastFired = std::chrono::high_resolution_clock::now();
  _lastReload = _lastFired;
}

Weapon::~Weapon(void) {}

void Weapon::reload(void) {
  const auto currentTime = std::chrono::high_resolution_clock::now();
  if (_ammo < 0 || currentTime - _lastReload < _reloadTime ||
      currentTime - _lastFired < _cd)
    return;
  if (_ammo >= _maxClip) {
    _clip = _maxClip;
    _ammo -= _maxClip;
  } else {
    _clip += _ammo;
    _ammo = 0;
  }
  _lastReload = std::chrono::high_resolution_clock::now();
  printf("Reload...\n");
}

bool Weapon::fire(void) {
  const auto currentTime = std::chrono::high_resolution_clock::now();
  if (_clip < 0 || currentTime - _lastReload < _reloadTime ||
      currentTime - _lastFired < _cd)
    return false;
  _clip--;
  _lastFired = currentTime;
  return true;
}

void Weapon::dealDamage(Entity &target, double dist) { target.injure(_damage); }

double Weapon::getRange(void) const { return _range; }
