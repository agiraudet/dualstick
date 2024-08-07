#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "Weapon.hpp"

#define MOVE_ABS

enum PlayerInput { UP, DOWN, LEFT, RIGHT, N_INPUT };

class Player : public Entity {
public:
  Player(void);
  Player(int id);
  ~Player(void);

  void setInput(PlayerInput input, bool state);
  void applyInput(void);

private:
  void _resetInputs(void);

public:
  Weapon *weapon;

private:
  bool _inputs[N_INPUT];
};

#endif
