#include "Entity.hpp"
#include "Map.hpp"
#include "Vector.hpp"
#include <bits/types/time_t.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ctime>

Entity::Entity(void)
    : weapon(nullptr), _alive(true), _id(-1), _position(0, 0), _velocity(0, 0),
      _maxSpeed(ENTITY_MAXSPEED), _size(ENTITY_SIZE), _hp(10) {
  _lastMove = std::chrono::high_resolution_clock::now();
}

Entity::Entity(int id)
    : weapon(nullptr), _alive(true), _id(id), _position(0, 0), _velocity(0, 0),
      _maxSpeed(ENTITY_MAXSPEED), _size(ENTITY_SIZE), _hp(10) {
  _lastMove = std::chrono::high_resolution_clock::now();
}

Entity::~Entity(void) {}

void Entity::setId(int id) { _id = id; }

void Entity::setPos(Vector &pos) { _position = pos; }

void Entity::setVel(Vector &vel) { _velocity = vel; }

void Entity::setAngle(float angle) { _angle = angle; }

void Entity::setMaxSpeed(float maxSpeed) { _maxSpeed = maxSpeed; }

void Entity::setLastUpdate(
    std::chrono::high_resolution_clock::time_point currentTime) {
  _lastMove = currentTime;
}

void Entity::setHp(int hp) {
  _hp = hp;
  if (_hp <= 0)
    _alive = false;
}

void Entity::setAlive(bool alive) { _alive = alive; }

int Entity::getId(void) const { return _id; }

Vector const &Entity::getPos(void) const { return _position; }

Vector const &Entity::getVel(void) const { return _velocity; }

float Entity::getAngle(void) const { return _angle; }

float Entity::getMaxSpeed(void) const { return _maxSpeed; }

int Entity::getSize(void) const { return _size; };

int Entity::getHp(void) const { return _hp; }

bool Entity::isAlive(void) const { return _alive; }

void Entity::update(Vector const &pos, Vector const &vel, float angle, int hp) {
  _position = pos;
  _velocity = vel;
  _angle = angle;
  _hp = hp;
  _lastMove = std::chrono::high_resolution_clock::now();
}

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
  Vector intendedPos = _position + _velocity * deltaTime;
  if (!map.boxIsColliding(intendedPos.x - (float)_size / 2.f,
                          intendedPos.y - (float)_size / 2.f, _size, _size)) {
    _position = intendedPos;
  } else {
    Vector slidePos = _position;
    slidePos.x = intendedPos.x;
    if (!map.boxIsColliding(slidePos.x - (float)_size / 2.f,
                            slidePos.y - (float)_size / 2.f, _size, _size)) {
      _position = slidePos;
    } else {
      slidePos = _position;
      slidePos.y = intendedPos.y;
      if (!map.boxIsColliding(slidePos.x - (float)_size / 2.f,
                              slidePos.y - (float)_size / 2.f, _size, _size)) {
        _position = slidePos;
      }
    }
  }
  _lastMove = currentTime;
}

void Entity::aimAngle(int targetX, int targetY) {
  float dx = targetX - _position.x;
  float dy = targetY - _position.y;
  _angle = std::atan2(dy, dx); //* (180.0f / M_PI);
}

void Entity::capSpeed(void) { _velocity.capIntensity(_maxSpeed * _maxSpeed); }

void Entity::capSpeed(int speed) { _velocity.capIntensity(speed); }

void Entity::injure(int damage) {
  _hp -= damage;
  if (_hp <= 0) {
    _hp = 0;
    _alive = false;
  }
}
