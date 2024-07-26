#include "Player.hpp"

Player::Player(void) : _id(0), _rect({0, 0, 50, 50}) {}

Player::Player(int id) : _id(id), _rect({0, 0, 50, 50}) {}

Player::Player(int x, int y) : _id(0), _rect({x, y, 50, 50}) {}

Player::Player(int id, int x, int y) : _id(id), _rect({x, y, 50, 50}) {}

Player::~Player(void) {}

void Player::move(int dx, int dy) {
  _rect.x += dx;
  _rect.y += dy;
}

void Player::updateRect(SDL_Rect &rect) { _rect = rect; }

SDL_Rect const &Player::getRect(void) const { return _rect; }

int Player::getId(void) const { return _id; }
