#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Map.hpp"
#include "Vector.hpp"
#include <chrono>

#define ENTITY_MAXSPEED 400.f
#define ENTITY_SIZE 20

class Entity {
public:
  Entity(void);
  Entity(int id);
  ~Entity(void);

  void move(void);
  void move(Map &map);
  void setId(int id);
  int getId(void) const;
  void setPos(Vector &pos);
  void setVel(Vector &vel);
  void setAngle(float angle);
  void aimAngle(int targetX, int targetY);
  float getAngle(void) const;
  Vector const &getPos(void) const;
  Vector const &getVel(void) const;
  int getSize(void) const;

protected:
  int _id;
  Vector _velocity;
  Vector _position;
  float _angle;
  float _maxSpeed;
  int _size;
  std::chrono::high_resolution_clock::time_point _lastMove;
};

#endif
