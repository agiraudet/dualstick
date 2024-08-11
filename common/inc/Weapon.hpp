#ifndef WEAPONHPP
#define WEAPONHPP

#include <chrono>

class Entity;

class Weapon {

public:
  Weapon(void);
  ~Weapon(void);

  bool reload(void);
  bool fire(void);
  void dealDamage(Entity &target, double dist);

public:
  bool unlimitedAmmo;
  int damage;
  double range;
  int ammo;
  int clip;
  int maxClip;
  std::chrono::milliseconds cd;
  std::chrono::milliseconds reloadTime;

private:
  std::chrono::high_resolution_clock::time_point _lastFired;
  std::chrono::high_resolution_clock::time_point _lastReload;
};

#endif //! WEAPONHPP
