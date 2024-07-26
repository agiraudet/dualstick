#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_video.h>
#include <unordered_map>

#include "Client.hpp"
#include "Player.hpp"

#define SCR_WIDTH 720
#define SCR_HEIGHT 480

class Engine {
public:
  Engine(void);
  ~Engine(void);

  void run(void);
  void setPlayerId(int id);
  void removePlayer(int id);
  void addPlayer(int id);
  void updatePlayer(int id, SDL_Rect &rect);

private:
  void _getEvent(void);
  void _render(void);
  void _processInput(SDL_Keycode &sym, bool state);

private:
  Client _client;
  bool _alive;
  Player _player;
  std::unordered_map<int, Player> _otherPlayers;
  SDL_Window *_window;
  SDL_Renderer *_renderer;
};

#endif
