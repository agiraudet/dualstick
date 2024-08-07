#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <string>
#include <unordered_map>

#include "Client.hpp"
#include "MapRend.hpp"
#include "Message.hpp"
#include "MobFactory.hpp"
#include "Player.hpp"
#include "Tex.hpp"

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

enum engState { STOPPED, RUNNING, LOADING };

class Engine {
public:
  Engine(void);
  ~Engine(void);

  int connect(const std::string &host = "localhost", enet_uint16 port = 45454);
  void run(void);

public:
  void receiveMsg(MessagePlayerCo *msg);
  void receiveMsg(MessagePlayerDisco *msg);
  void receiveMsg(MessagePlayerUpdate *msg);
  void receiveMsg(MessagePlayerID *msg);
  void receiveMsg(MessageMobUpdate *msg);
  void receiveMsg(MessageGameState *msg);
  void receiveMsg(MessageMap *msg);

private:
  void _removePlayer(int id);
  void _addPlayer(int id);
  void _updatePlayer(int id, Vector &pos, Vector &vel, float angle);
  void _fillAtlas(void);
  void _getEvent(void);
  void _render(void);
  void _processInput(SDL_Keycode &sym, bool state);
  void _NotifyPlayerUpdate(void);
  void _centerCameraOnPlayer(void);

private:
  Client _client;
  engState _state;
  Player _player;
  MessagePlayerUpdate _msgPlayerUpdate;
  std::unordered_map<int, Player> _otherPlayers;
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  SDL_Rect _camera;
  MapRend *_map;
  MobFactory _hive;
  std::unordered_map<std::string, Tex> _atlas;
};

#endif
