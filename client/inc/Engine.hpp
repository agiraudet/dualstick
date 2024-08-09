#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>
#include <string>

#include "Client.hpp"
#include "DisplayManager.hpp"
#include "Entity.hpp"
#include "Map.hpp"
#include "Message.hpp"
#include "Player.hpp"

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
  void receiveMsg(MessagePlayerID *msg);
  void receiveMsg(MessagePlayerDead *msg);
  void receiveMsg(MessageMobHit *msg);
  void receiveMsg(MessageMobAttack *msg);
  void receiveMsg(MessageGameState *msg);
  void receiveMsg(MessageMap *msg);

private:
  void _removePlayer(int id);
  void _addPlayer(int id);
  void _updatePlayer(int id, Vector &pos, Vector &vel, float angle, int hp);
  void _updateEntity(std::shared_ptr<Entity> entity, MessageEntityUpdate &msg);
  void _debugUpdateEntity(std::shared_ptr<Entity> entity,
                          MessageEntityUpdate &msg);
  void _fillAtlas(void);
  void _getEvent(void);
  void _processInput(SDL_Keycode &sym, bool state);
  void _NotifyPlayerUpdate(void);

private:
  Client _client;
  DisplayManager _dm;
  engState _state;
  MessageEntityUpdate _msgPlayerUpdate;
  Map *_map;
  std::shared_ptr<Player> _player;
  EntityManager<Player> _EMPlayers;
  EntityManager<Mob> _EMMobs;
};

#endif
