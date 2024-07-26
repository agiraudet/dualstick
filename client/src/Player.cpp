#include "Player.hpp"
#include <iostream>

Player::Player(void)
    : _id(0), _rect({0, 0, 50, 50}), _x(0), _y(0), _vX(0), _vY(0),
      _maxSpeed(PLAYER_MAXSPEED) {
  _resetInputs();
}

Player::Player(int id)
    : _id(id), _rect({0, 0, 50, 50}), _x(0), _y(0), _vX(0), _vY(0),
      _maxSpeed(PLAYER_MAXSPEED) {
  _resetInputs();
}

Player::Player(int x, int y)
    : _id(0), _rect({x, y, 50, 50}), _x(0), _y(0), _vX(0), _vY(0),
      _maxSpeed(PLAYER_MAXSPEED) {
  _resetInputs();
}

Player::Player(int id, int x, int y)
    : _id(id), _rect({x, y, 50, 50}), _x(0), _y(0), _vX(0), _vY(0),
      _maxSpeed(PLAYER_MAXSPEED) {
  _resetInputs();
}

Player::~Player(void) {}

void Player::setId(int id) { _id = id; }

void Player::move(void) {
  _rect.x += _vX;
  _rect.y += _vY;
  std::cout << "(" << _rect.x << "," << _rect.y << ") "
            << "[" << _vX << "," << _vY << "]" << std::endl;
}

void Player::updateRect(SDL_Rect &rect) {
  _rect.x = rect.x;
  _rect.y = rect.y;
  _rect.w = rect.w;
  _rect.h = rect.h;
}

void Player::updatePos(float x, float y) {
  _x = x;
  _y = y;
}

SDL_Rect const &Player::getRect(void) const { return _rect; }

int Player::getId(void) const { return _id; }

float Player::getX(void) const { return _x; }

float Player::getY(void) const { return _x; }

void Player::incVelX(int dir) {
  dir = dir < 0 ? -1 : 1;
  _vX = _maxSpeed * dir;
}

void Player::incVelY(int dir) {
  dir = dir < 0 ? -1 : 1;
  _vY = _maxSpeed * dir;
}

void Player::capSpeed(void) {
  if (_vX + _vY > _maxSpeed) {
    _vX /= 2;
    _vX /= 2;
  }
}

void Player::setInput(PlayerInput input, bool state) {
  std::cout << input << " -> " << state << std::endl;
  _inputs[input] = state;
}

void Player::applyInput(void) {
  _vX = 0;
  _vY = 0;
  if (_inputs[UP])
    incVelY(-1);
  if (_inputs[DOWN])
    incVelY(1);
  if (_inputs[LEFT])
    incVelX(-1);
  if (_inputs[RIGHT])
    incVelX(1);
  capSpeed();
  move();
}

void Player::_resetInputs(void) {
  for (int i = 0; i < N_INPUT; i++)
    _inputs[i] = false;
}
