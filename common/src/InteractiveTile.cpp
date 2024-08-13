#include "InteractiveTile.hpp"

InteractiveTile::InteractiveTile() : _x(0), _y(0) {}

InteractiveTile::InteractiveTile(int x, int y) : _x(x), _y(y) {}

InteractiveTile::InteractiveTile(const InteractiveTile &other)
    : _x(other._x), _y(other._y) {}

InteractiveTile &InteractiveTile::operator=(const InteractiveTile &other) {
  if (this != &other) {
    _x = other._x;
    _y = other._y;
  }
  return *this;
}

InteractiveTile::~InteractiveTile() {}

int InteractiveTile::getX() const { return _x; }

int InteractiveTile::getY() const { return _y; }

void InteractiveTile::setPos(int x, int y) {
  _x = x;
  _y = y;
}

bool InteractiveTile::isInRange(int x, int y) const {
  return (x == _x || x == _x + 1 || x == _x - 1) &&
         (y == _y || y == _y + 1 || y == _y - 1);
}
