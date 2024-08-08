#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Entity.hpp"
#include <chrono>

class Weapon {

public:
  Weapon(void);
  ~Weapon(void);

  void reload(void);
  bool fire(void);
  void dealDamage(Entity &target, double dist);
  double getRange(void) const;
  int getAmmo(void) const;
  int getClip(void) const;

private:
  int _damage;
  double _range;
  int _ammo;
  int _clip;
  int _maxClip;
  std::chrono::milliseconds _cd;
  std::chrono::milliseconds _reloadTime;
  std::chrono::high_resolution_clock::time_point _lastFired;
  std::chrono::high_resolution_clock::time_point _lastReload;
};

#endif //! WEAPON_HPP
