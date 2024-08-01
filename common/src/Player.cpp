#include "Player.hpp"
#include <bits/types/time_t.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ctime>

Player::Player(void) : _id(0), _maxSpeed(PLAYER_MAXSPEED), _size(PLAYER_SIZE) {
  _resetInputs();
  _lastMove = std::chrono::high_resolution_clock::now();
}

Player::Player(int id)
    : _id(id), _maxSpeed(PLAYER_MAXSPEED), _size(PLAYER_SIZE) {
  _resetInputs();
  _lastMove = std::chrono::high_resolution_clock::now();
}

Player::~Player(void) {}

void Player::setId(int id) { _id = id; }

void Player::move(void) {
  auto currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> timeSinceMove =
      currentTime - _lastMove;
  double deltaTime = timeSinceMove.count() / 1000.0;
  _position += _velocity * deltaTime;
  _lastMove = currentTime;
}

void Player::move(Map &map) {
  auto currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> timeSinceMove =
      currentTime - _lastMove;
  double deltaTime = timeSinceMove.count() / 1000.0;
  Vector futurPos = _position + _velocity * deltaTime;
  if (!map.boxIsColliding(futurPos.x - (float)_size / 2.f,
                          futurPos.y - (float)_size / 2.f, _size, _size)) {
    _position = futurPos;
  }
  _lastMove = currentTime;
}

int Player::getId(void) const { return _id; }

void Player::setPos(Vector &pos) { _position = pos; }

void Player::setVel(Vector &vel) { _velocity = vel; }

Vector const &Player::getPos(void) const { return _position; }

Vector const &Player::getVel(void) const { return _velocity; }

int Player::getSize(void) const { return _size; };

float Player::getAngle(void) const { return _angle; }

void Player::setAngle(float angle) { _angle = angle; }

void Player::aimAngle(int targetX, int targetY) {
  float dx = targetX - _position.x;
  float dy = targetY - _position.y;
  _angle = std::atan2(dy, dx) * (180.0f / M_PI);
}

void Player::setInput(PlayerInput input, bool state) { _inputs[input] = state; }

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
  _velocity.capIntensity(_maxSpeed * _maxSpeed);
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

void Player::_resetInputs(void) {
  for (int i = 0; i < N_INPUT; i++)
    _inputs[i] = false;
}
