#ifndef INTERACTIVE_TILE_HPP
#define INTERACTIVE_TILE_HPP

#include "Player.hpp"

class InteractiveTile {
public:
  InteractiveTile();
  InteractiveTile(int x, int y);
  InteractiveTile(const InteractiveTile &other);
  InteractiveTile &operator=(const InteractiveTile &other);
  ~InteractiveTile();

  int getX() const;
  int getY() const;
  void setPos(int x, int y);
  bool isInRange(int x, int y) const;
  virtual void interact(Player &player) = 0;

protected:
  int _x;
  int _y;
};

#endif // INTERACTIVE_TILE_HPP
