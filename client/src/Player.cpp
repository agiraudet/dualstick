#include "Player.hpp"
#include "msg.hpp"

Player::Player(void) : _id(0), _rect({0, 0, 50, 50}) {}

Player::Player(int id) : _id(id), _rect({0, 0, 50, 50}) {}

Player::Player(int x, int y) : _id(0), _rect({x, y, 50, 50}) {}

Player::Player(int id, int x, int y) : _id(id), _rect({x, y, 50, 50}) {}

Player::~Player(void) {}

void Player::move(int dx, int dy) {
  _rect.x += dx;
  _rect.y += dy;
}

SDL_Rect const &Player::getRect(void) const { return _rect; }

t_msgplr const &Player::getMsg(void) {
  _msg.rect = _rect;
  _msg.id = _id;
  return _msg;
}
