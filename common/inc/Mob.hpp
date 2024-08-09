#ifndef MOB_HPP
#define MOB_HPP

#include "Entity.hpp"
#include "Player.hpp"
#include "Vector.hpp"

class Mob : public Entity {

public:
  Mob(void);
  Mob(int id);
  ~Mob(void);

  Vector const &getTarget(void) const;
  std::shared_ptr<Player> findClosest(EntityManager<Player> const &EMPlayer);
  inline int getTileX(void) const { return _position.x / 32; }
  inline int getTileY(void) const { return _position.y / 32; }

private:
  void _initWeapon(void);

private:
  Vector _target;
};

#endif
