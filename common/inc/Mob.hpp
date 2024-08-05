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
  inline int getTileX(void) const { return _position.x / 32; }
  inline int getTileY(void) const { return _position.y / 32; }

private:
  mobType _type;
  int _tileX;
  int _tileY;
  Vector _target;
};

#endif
