#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Vector.hpp"

#define PLAYER_MAXSPEED 8.f

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
  float _maxSpeed;
  bool _inputs[N_INPUT];
};

#endif
