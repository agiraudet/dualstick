#include "Engine.hpp"
#include "MapRend.hpp"
#include "Message.hpp"
#include "Timer.hpp"
#include "Vector.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <enet/enet.h>
#include <enet/types.h>
#include <exception>
#include <iostream>

Engine::Engine(void)
    : _client(Client()), _state(LOADING),
      _msgPlayerUpdate({-1, 0.f, Vector(-99, -99), Vector(-99, -99)}),
      _window(nullptr), _renderer(nullptr),
      _camera({0, 0, SCR_WIDTH, SCR_HEIGHT}) {
  SDL_Init(SDL_INIT_VIDEO);
  _window = SDL_CreateWindow("Client", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT,
                             SDL_WINDOW_SHOWN);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  _fillAtlas();
  _map = new MapRend();
  Vector initPos(64.f, 64.f);
  _player.setPos(initPos);
}

Engine::~Engine(void) {
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

int Engine::connect(const std::string &host, enet_uint16 port) {
  try {
    _client.init(host, port);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  return 0;
}

void Engine::run(void) {
  TimerFps fpsTimer(60.f);
  while (_state != STOPPED) {
    _client.getEvent(*this);
    _getEvent();
    if (_state == RUNNING) {
      for (auto &p : _otherPlayers) {
        p.second.move();
      }
      for (auto &m : _hive.getMobs()) {
        m.second->move();
      }
      _player.applyInput();
      _player.move(*_map);
      _centerCameraOnPlayer();
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      _player.aimAngle(mouseX + _camera.x, mouseY + _camera.y);
      _NotifyPlayerUpdate();
      _render();
    }
    fpsTimer.capFps();
  }
}

void Engine::_fillAtlas(void) {
  _atlas["other"] = Tex(_renderer, "../assets/other.png");
  _atlas["player"] = Tex(_renderer, "../assets/player.png");
  _atlas["mob"] = Tex(_renderer, "../assets/mob.png");
  _atlas["tiles"] = Tex(_renderer, "../assets/tileset.png", 32);
  _anims["anim"] = Anim(_renderer, "../assets/anim.png", 16);
  /*_anims["anim"].setLoop(true);*/
  _anims["anim"].setFps(120);
  /*_anims["anim"].start();*/
}

void Engine::_getEvent(void) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      _state = STOPPED;
    else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
      _processInput(e.key.keysym.sym, (e.type == SDL_KEYDOWN));
  }
}

void Engine::_processInput(SDL_Keycode &sym, bool state) {
  switch (sym) {
  case SDLK_w:
  case SDLK_UP:
    _player.setInput(UP, state);
    break;
  case SDLK_s:
  case SDLK_DOWN:
    _player.setInput(DOWN, state);
    break;
  case SDLK_a:
  case SDLK_LEFT:
    _player.setInput(LEFT, state);
    break;
  case SDLK_d:
  case SDLK_RIGHT:
    _player.setInput(RIGHT, state);
    break;
  case SDLK_SPACE:
    if (_player.weapon->fire()) {
      ENetPacket *pck = packageMessage(_msgPlayerUpdate, PLR_SHOOT, true);
      std::cout << "pew" << std::endl;
      _client.sendMessage(pck);
      _anims["anim"].start();
    }
    break;
  }
}

void Engine::_centerCameraOnPlayer(void) {
  _camera.x = (_player.getPos().x + (float)_player.getSize() / 2) -
              (float)SCR_WIDTH / 2.f;
  _camera.y = (_player.getPos().y + (float)_player.getSize() / 2) -
              (float)SCR_HEIGHT / 2.f;
}

void Engine::_render(void) {
  SDL_SetRenderDrawColor(_renderer, 63, 56, 81, 255);
  SDL_RenderClear(_renderer);

  _map->render(_camera, _atlas["tiles"]);
  for (const auto &p : _hive.getMobs()) {
    const auto &mob = p.second;
    _atlas["mob"].drawRot(mob->getPos().x - mob->getSize() / 2 - _camera.x,
                          mob->getPos().y - mob->getSize() / 2 - _camera.y,
                          mob->getAngle() * (180.0f / M_PI));
  }
  _atlas["player"].drawRot(
      _player.getPos().x - _player.getSize() / 2 - _camera.x,
      _player.getPos().y - _player.getSize() / 2 - _camera.y,
      _player.getAngle() * (180.0f / M_PI));
  for (const auto &p : _otherPlayers) {
    _atlas["other"].drawRot(
        p.second.getPos().x - p.second.getSize() / 2 - _camera.x,
        p.second.getPos().y - p.second.getSize() / 2 - _camera.y,
        p.second.getAngle() * (180.0f / M_PI));
  }
  // TEST //
  float offsetDistance = _player.getSize() * 0.75;
  float playerAngleRadians = _player.getAngle();
  float offsetX = cos(playerAngleRadians) * offsetDistance;
  float offsetY = sin(playerAngleRadians) * offsetDistance;
  float animPosX =
      _player.getPos().x + offsetX - _player.getSize() / 2 - _camera.x;
  float animPosY =
      _player.getPos().y + offsetY - _player.getSize() / 2 - _camera.y;
  _anims["anim"].drawRot(animPosX, animPosY,
                         playerAngleRadians * (180.0f / M_PI) + 90.f);
  ///////////
  SDL_RenderPresent(_renderer);
}

void Engine::_NotifyPlayerUpdate(void) {
  Vector const &plrPos = _player.getPos();
  Vector const &plrVel = _player.getVel();
  float plrAngle = _player.getAngle();
  _msgPlayerUpdate.id = _player.getId();
  if (_msgPlayerUpdate.angle != plrAngle || _msgPlayerUpdate.pos != plrPos ||
      _msgPlayerUpdate.vel != plrVel) {
    _msgPlayerUpdate.angle = plrAngle;
    _msgPlayerUpdate.pos = plrPos;
    _msgPlayerUpdate.vel = plrVel;
    ENetPacket *pck = packageMessage(_msgPlayerUpdate, PLR_UPDATE, true);
    _client.sendMessage(pck);
  }
}

void Engine::_removePlayer(int id) {
  if (_otherPlayers.find(id) != _otherPlayers.end()) {
    _otherPlayers.erase(id);
  }
}

void Engine::_addPlayer(int id) {
  if (_otherPlayers.find(id) != _otherPlayers.end()) {
    _otherPlayers.erase(id);
  }
  _otherPlayers[id] = Player(id);
}

void Engine::_updatePlayer(int id, Vector &pos, Vector &vel, float angle) {
  if (id == _player.getId())
    return;
  std::unordered_map<int, Player>::iterator it = _otherPlayers.find(id);
  if (it == _otherPlayers.end())
    _addPlayer(id);
  _otherPlayers[id].setPos(pos);
  _otherPlayers[id].setVel(vel);
  _otherPlayers[id].setAngle(angle);
  _otherPlayers[id].setLastUpdate(std::chrono::high_resolution_clock::now());
}

void Engine::receiveMsg(MessagePlayerCo *msg) {
  if (msg->id != _player.getId())
    _addPlayer(msg->id);
}

void Engine::receiveMsg(MessagePlayerDisco *msg) { _removePlayer(msg->id); }

void Engine::receiveMsg(MessagePlayerUpdate *msg) {
  _updatePlayer(msg->id, msg->pos, msg->vel, msg->angle);
}

void Engine::receiveMsg(MessagePlayerID *msg) {
  _player.setId(msg->id);
  printf("[Player] Got assigned id: %d\n", msg->id);
}

void Engine::receiveMsg(MessageMobUpdate *msg) {
  _hive.updateMob(msg->id, msg->type, msg->pos, msg->vel, msg->angle);
}

void Engine::receiveMsg(MessageGameState *msg) {
  static uint32_t lastStamp = 0;
  if (msg->stamp <= lastStamp)
    return;
  lastStamp = msg->stamp;
  for (int i = 0; i < msg->nplayer && i < MAX_N_PLAYER; i++)
    receiveMsg(&msg->players[i]);
  if (msg->nmob < _hive.getMobs().size())
    _hive.removeAbsentMobs(msg->mob, msg->nmob);
  for (int i = 0; i < msg->nmob && i < MAX_N_MOB; i++)
    receiveMsg(&msg->mob[i]);
}

void Engine::receiveMsg(MessageMap *msg) {
  _map->rcvMsg(*msg);
  if (_map->countMissingMsg() == 0 && !_map->isLoaded()) {
    if (_map->loadFromMsg()) {
      _map->createTiles();
      _state = RUNNING;
      printf("[Map] Loaded successfully\n");
    }
  }
}
