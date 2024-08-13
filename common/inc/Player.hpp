#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"

#define MOVE_ABS

enum PlayerInput { UP, DOWN, LEFT, RIGHT, N_INPUT };

class Player : public Entity {
public:
  Player(void);
  Player(int id);
  ~Player(void);

  void setInput(PlayerInput input, bool state);
  void applyInput(void);
  int getMoney(void) const;
  void addMoney(int money);
  bool removeMoney(int money);
  void setMoney(int money);

private:
  void _resetInputs(void);

private:
  bool _inputs[N_INPUT];
  int _money;
};

#endif
