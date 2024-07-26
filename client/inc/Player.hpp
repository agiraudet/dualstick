#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>

#define PLAYER_MAXSPEED 1.f

enum PlayerInput { UP, DOWN, LEFT, RIGHT, N_INPUT };

class Player {
public:
  Player(void);
  Player(int id);
  Player(int x, int y);
  Player(int id, int x, int y);
  ~Player(void);

  void move(void);
  void updateRect(SDL_Rect &rect);
  void updatePos(float x, float y);
  SDL_Rect const &getRect(void) const;
  void setId(int id);
  int getId(void) const;
  float getX(void) const;
  float getY(void) const;
  void incVelX(int dir);
  void incVelY(int dir);
  void capSpeed(void);
  void setInput(PlayerInput input, bool state);
  void applyInput(void);

private:
  void _resetInputs(void);

private:
  int _id;
  SDL_Rect _rect;
  float _x;
  float _y;
  float _vX;
  float _vY;
  int _maxSpeed;
  bool _inputs[N_INPUT];
};

#endif
