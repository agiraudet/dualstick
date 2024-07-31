#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Vector.hpp"
#include <chrono>

#define PLAYER_MAXSPEED 800.f

#define MOVE_ABS

enum PlayerInput { UP, DOWN, LEFT, RIGHT, N_INPUT };

class Player {
public:
  Player(void);
  Player(int id);
  ~Player(void);

  void move(void);
  void setId(int id);
  int getId(void) const;
  void setPos(Vector &pos);
  void setVel(Vector &vel);
  void setAngle(float angle);
  void aimAngle(int targetX, int targetY);
  float getAngle(void) const;
  Vector const &getPos(void) const;
  Vector const &getVel(void) const;
  void setInput(PlayerInput input, bool state);
  void applyInput(void);

private:
  void _resetInputs(void);

private:
  int _id;
  Vector _velocity;
  Vector _position;
  float _angle;
  float _maxSpeed;
  bool _inputs[N_INPUT];
  std::chrono::high_resolution_clock::time_point _lastMove;
};

#endif
