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
  void updateRect(SDL_Rect &rect);
  SDL_Rect const &getRect(void) const;
  int getId(void) const;

private:
  int _id;
  SDL_Rect _rect;
};

#endif
