#include "Player.hpp"
#include "Entity.hpp"
#include "Weapon.hpp"
#include <cmath>
#include <cstdio>
#include <ctime>

Player::Player(void) : Entity() {
  _resetInputs();
  weapon = new Weapon();
}

Player::Player(int id) : Entity() {
  _id = id;
  _resetInputs();
  weapon = new Weapon();
}

Player::~Player(void) {}

void Player::setInput(PlayerInput input, bool state) { _inputs[input] = state; }

void Player::_resetInputs(void) {
  for (int i = 0; i < N_INPUT; i++)
    _inputs[i] = false;
}

#ifdef MOVE_ABS
void Player::applyInput(void) {
  float speed = _maxSpeed;
  _velocity.setXY(0.f, 0.f);
  if (_inputs[UP])
    _velocity += Vector(0, -speed);
  if (_inputs[DOWN])
    _velocity += Vector(0, speed);
  if (_inputs[LEFT])
    _velocity += Vector(-speed, 0);
  if (_inputs[RIGHT])
    _velocity += Vector(speed, 0);
  capSpeed();
}
#else
void Player::applyInput(void) {
  float speed = _maxSpeed;
  _velocity.setXY(0.f, 0.f);
  float radianAngle = _angle * (M_PI / 180.0f);
  if (_inputs[UP]) {
    _velocity += Vector(cos(radianAngle) * speed, sin(radianAngle) * speed);
  }
  if (_inputs[DOWN]) {
    _velocity += Vector(-cos(radianAngle) * speed, -sin(radianAngle) * speed);
  }
  if (_inputs[LEFT]) {
    float leftAngle = radianAngle - M_PI / 2.0f; // 90 degrees to the left
    _velocity += Vector(cos(leftAngle) * speed, sin(leftAngle) * speed);
  }
  if (_inputs[RIGHT]) {
    float rightAngle = radianAngle + M_PI / 2.0f; // 90 degrees to the right
    _velocity += Vector(cos(rightAngle) * speed, sin(rightAngle) * speed);
  }
  _velocity.capIntensity(_maxSpeed * _maxSpeed);
}
#endif // MOVE_ABS
