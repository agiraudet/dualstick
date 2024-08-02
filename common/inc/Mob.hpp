#ifndef MOB_HPP
#define MOB_HPP

#include "Message.hpp"
#include "Player.hpp"
#include "Vector.hpp"

class Mob : public Entity {
  static int count;

public:
  Mob(void);
  void setType(mobType type);
  mobType getType(void) const;
  void findClosest(std::vector<Player *> &playerVec);

private:
  mobType _type;
  Vector _target;
};

#endif
