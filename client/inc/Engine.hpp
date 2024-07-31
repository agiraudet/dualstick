#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unordered_map>

#include "Atlas.hpp"
#include "Client.hpp"
#include "MapRend.hpp"
#include "Message.hpp"
#include "Player.hpp"

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

class Engine {
public:
  Engine(void);
  ~Engine(void);

  int connect(const std::string &host = "localhost", enet_uint16 port = 45454);
  void run(void);
  void setPlayerId(int id);
  void removePlayer(int id);
  void addPlayer(int id);
  void updatePlayer(MessagePlayerUpdate *msg);

private:
  void _fillAtlas(void);
  void _getEvent(void);
  void _render(void);
  void _processInput(SDL_Keycode &sym, bool state);
  void _NotifyPlayerUpdate(void);
  void _centerCameraOnPlayer(void);

private:
  Client _client;
  bool _alive;
  Player _player;
  MessagePlayerUpdate _msgPlayerUpdate;
  std::unordered_map<int, Player> _otherPlayers;
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  Atlas *_atlas;
  SDL_Rect _camera;
  MapRend *_map;
};

#endif
