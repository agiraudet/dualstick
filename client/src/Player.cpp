#include "Player.hpp"

Player::Player(void) : _id(0), _maxSpeed(PLAYER_MAXSPEED) { _resetInputs(); }

Player::Player(int id) : _id(id), _maxSpeed(PLAYER_MAXSPEED) { _resetInputs(); }

Player::~Player(void) {}

void Player::setId(int id) { _id = id; }

void Player::move(void) { _position += _velocity; }

int Player::getId(void) const { return _id; }

void Player::setPos(Vector &pos) { _position = pos; }

void Player::setVel(Vector &vel) { _velocity = vel; }

Vector const &Player::getPos(void) const { return _position; }

Vector const &Player::getVel(void) const { return _velocity; }

void Player::setInput(PlayerInput input, bool state) { _inputs[input] = state; }

void Player::applyInput(void) {
  float speed = _maxSpeed / 2;
  _velocity.setXY(0.f, 0.f);
  if (_inputs[UP])
    _velocity += Vector(0, -speed);
  if (_inputs[DOWN])
    _velocity += Vector(0, speed);
  if (_inputs[LEFT])
    _velocity += Vector(-speed, 0);
  if (_inputs[RIGHT])
    _velocity += Vector(speed, 0);
  _velocity.capIntensity(_maxSpeed);
  move();
}

void Player::_resetInputs(void) {
  for (int i = 0; i < N_INPUT; i++)
    _inputs[i] = false;
}
