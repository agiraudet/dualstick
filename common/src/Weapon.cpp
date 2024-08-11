#include "Weapon.hpp"
#include "Entity.hpp"
#include <chrono>
#include <cstdio>

Weapon::Weapon(void)
    : unlimitedAmmo(false), damage(5), range(3000), ammo(100), clip(10),
      maxClip(10), cd(200), reloadTime(1000) {
  _lastFired = std::chrono::high_resolution_clock::now();
  _lastReload = _lastFired;
}

Weapon::~Weapon(void) {}

bool Weapon::reload(void) {
  if (unlimitedAmmo || clip >= maxClip)
    return false;
  const auto currentTime = std::chrono::high_resolution_clock::now();
  if (ammo <= 0 || currentTime - _lastReload < reloadTime ||
      currentTime - _lastFired < cd)
    return false;
  if (ammo >= maxClip) {
    clip = maxClip;
    ammo -= maxClip;
  } else {
    clip += ammo;
    ammo = 0;
  }
  _lastReload = std::chrono::high_resolution_clock::now();
  return true;
}

bool Weapon::fire(void) {
  const auto currentTime = std::chrono::high_resolution_clock::now();
  if (clip <= 0 || currentTime - _lastReload < reloadTime ||
      currentTime - _lastFired < cd)
    return false;
  clip--;
  _lastFired = currentTime;
  return true;
}

void Weapon::dealDamage(Entity &target, double dist) { target.injure(damage); }
