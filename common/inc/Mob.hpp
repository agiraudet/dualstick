#ifndef MOB_HPP
#define MOB_HPP

#include "Entity.hpp"
#include "Player.hpp"
#include "Vector.hpp"
#include <memory>

class Mob : public Entity {

public:
  Mob(void);
  Mob(int id);
  ~Mob(void);

  std::shared_ptr<Player> getTarget(void) const;
  std::shared_ptr<Player> findClosest(EntityManager<Player> const &EMPlayer);
  void processDir(Vector aim, int tileSize);
  bool tryHitTarget(void);
  void setVelTowards(Vector target);

private:
  void _initWeapon(void);

private:
  std::shared_ptr<Player> _target;
};

#endif
