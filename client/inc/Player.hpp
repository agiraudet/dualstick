#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>

class Player {
public:
  Player(void);
  Player(int id);
  Player(int x, int y);
  Player(int id, int x, int y);
  ~Player(void);

  void move(int dx, int dy);
  SDL_Rect const &getRect(void) const;
  t_msgplr const &getMsg(void);

private:
  int _id;
  SDL_Rect _rect;
  t_msgplr _msg;
};

#endif
