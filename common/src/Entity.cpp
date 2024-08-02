#include "Entity.hpp"
#include <bits/types/time_t.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ctime>

Entity::Entity(void) : _id(0), _maxSpeed(ENTITY_MAXSPEED), _size(ENTITY_SIZE) {
  _lastMove = std::chrono::high_resolution_clock::now();
}

Entity::Entity(int id)
    : _id(id), _maxSpeed(ENTITY_MAXSPEED), _size(ENTITY_SIZE) {
  _lastMove = std::chrono::high_resolution_clock::now();
}

Entity::~Entity(void) {}

void Entity::setId(int id) { _id = id; }

void Entity::move(void) {
  auto currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> timeSinceMove =
      currentTime - _lastMove;
  double deltaTime = timeSinceMove.count() / 1000.0;
  _position += _velocity * deltaTime;
  _lastMove = currentTime;
}

void Entity::move(Map &map) {
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

int Entity::getId(void) const { return _id; }

void Entity::setPos(Vector &pos) { _position = pos; }

void Entity::setVel(Vector &vel) { _velocity = vel; }

Vector const &Entity::getPos(void) const { return _position; }

Vector const &Entity::getVel(void) const { return _velocity; }

int Entity::getSize(void) const { return _size; };

float Entity::getAngle(void) const { return _angle; }

void Entity::setAngle(float angle) { _angle = angle; }

void Entity::aimAngle(int targetX, int targetY) {
  float dx = targetX - _position.x;
  float dy = targetY - _position.y;
  _angle = std::atan2(dy, dx) * (180.0f / M_PI);
}
