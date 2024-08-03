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
  Player *findClosest(std::vector<Player *> &playerVec);
  bool futurMovAllowed(Map &map, Vector const &vel);

private:
  mobType _type;
  Vector _target;
};

#endif
