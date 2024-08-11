#include "Engine.hpp"
#include "DisplayManager.hpp"
#include "Entity.hpp"
#include "Message.hpp"
#include "Timer.hpp"
#include "Vector.hpp"

#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <emmintrin.h>
#include <enet/enet.h>
#include <enet/types.h>
#include <exception>
#include <iostream>
#include <unordered_set>

Engine::Engine(void)
    : _client(Client()), _state(LOADING),
      _msgPlayerUpdate({-1, 0.f, Vector(-99, -99), Vector(-99, -99), 10}) {
  _map = new Map();
  _player = nullptr;
}

Engine::~Engine(void) {}

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
      for (auto &[id, player] : _EMPlayers) {
        if (id != _player->getId())
          player->move();
      }
      for (auto &[id, mob] : _EMMobs) {
        mob->move();
      }
      if (_player && _player->isAlive()) {
        _player->applyInput();
        _player->move(*_map);
        _dm.centerCamera(*_player);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        _player->aimAngle(mouseX + _dm.getCamX(), mouseY + _dm.getCamY());
        _NotifyPlayerUpdate();
      }
      _dm.renderFrame(_player->getId(), _EMPlayers, _EMMobs);
    }
    fpsTimer.capFps();
  }
}

void Engine::_getEvent(void) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      _state = STOPPED;
    else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
      if (_player)
        _processInput(e.key.keysym.sym, (e.type == SDL_KEYDOWN));
    }
  }
}

void Engine::_processInput(SDL_Keycode &sym, bool state) {
  switch (sym) {
  case SDLK_w:
  case SDLK_UP:
    _player->setInput(UP, state);
    break;
  case SDLK_s:
  case SDLK_DOWN:
    _player->setInput(DOWN, state);
    break;
  case SDLK_a:
  case SDLK_LEFT:
    _player->setInput(LEFT, state);
    break;
  case SDLK_d:
  case SDLK_RIGHT:
    _player->setInput(RIGHT, state);
    break;
  case SDLK_SPACE:
    if (_player->weapon->fire()) {
      MessagePlayerShoot msg = {_player->getId()};
      ENetPacket *pck = packageMessage(msg, PLR_SHOOT, true);
      _client.sendMessage(pck);
      _dm.addEntityFx(*_player, SHOOT, _player->getSize() * 0.75, 90.f).start();
    }
    break;
  case SDLK_r:
    if (_player->weapon->reload()) {
      MessagePlayerReload msg = {_player->getId()};
      ENetPacket *pck = packageMessage(msg, PLR_RELOAD, true);
      _client.sendMessage(pck);
    }
    break;
  case SDLK_F1:
    std::cout << _player->getPos() << std::endl;
    break;
  }
}

void Engine::_NotifyPlayerUpdate(void) {
  Vector const &plrPos = _player->getPos();
  Vector const &plrVel = _player->getVel();
  float plrAngle = _player->getAngle();
  _msgPlayerUpdate.id = _player->getId();
  if (_msgPlayerUpdate.angle != plrAngle || _msgPlayerUpdate.pos != plrPos ||
      _msgPlayerUpdate.vel != plrVel) {
    _msgPlayerUpdate.angle = plrAngle;
    _msgPlayerUpdate.pos = plrPos;
    _msgPlayerUpdate.vel = plrVel;
    _msgPlayerUpdate.hp = _player->getHp();
    ENetPacket *pck = packageMessage(_msgPlayerUpdate, PLR_UPDATE, true);
    _client.sendMessage(pck);
  }
}

void Engine::_removePlayer(int id) { _EMPlayers.remove(id); }

void Engine::_addPlayer(int id) { _EMPlayers.create(id); }

void Engine::_updatePlayer(int id, Vector &pos, Vector &vel, float angle,
                           int hp) {
  _EMPlayers.get(id)->update(pos, vel, angle, hp);
}

void Engine::_updateEntity(std::shared_ptr<Entity> entity,
                           MessageEntityUpdate &msg) {
  if (!entity)
    return;
  entity->update(msg.pos, msg.vel, msg.angle, msg.hp);
}
void Engine::_debugUpdateEntity(std::shared_ptr<Entity> entity,
                                MessageEntityUpdate &msg) {
  if (!entity)
    return;
  entity->setHp(msg.hp);
}

void Engine::receiveMsg(MessagePlayerCo *msg) { _addPlayer(msg->id); }

void Engine::receiveMsg(MessagePlayerDisco *msg) { _removePlayer(msg->id); }

void Engine::receiveMsg(MessagePlayerID *msg) {
  // ID should not create a player...
  printf("[Player] Got assigned id: %d\n", msg->id);
  _player = _EMPlayers.create(msg->id);
  Vector pos(64, 64);
  _player->setPos(pos);
}

void Engine::receiveMsg(MessagePlayerDead *msg) {
  _EMPlayers.get(msg->id)->setAlive(false);
}

void Engine::receiveMsg(MessageMobHit *msg) {
  auto mob = _EMMobs.get(msg->id);
  if (mob)
    _dm.addStaticFx(DAMAGE, mob->getPos().x - (float)mob->getSize() / 2,
                    mob->getPos().y - (float)mob->getSize() / 2)
        .start();
}

void Engine::receiveMsg(MessageMobAttack *msg) {
  auto player = _EMPlayers.get(msg->playerId);
  if (player)
    _dm.addEntityFx(*player, MOB_ATCK, (float)player->getSize() / 2).start();
}

void Engine::receiveMsg(MessageGameState *msg) {
  static uint32_t lastStamp = 0;
  if (msg->stamp <= lastStamp)
    return;
  lastStamp = msg->stamp;
  int i = 0;
  for (; i < msg->nplayer; i++) {
    if (_player && msg->entity[i].id == _player->getId())
      _debugUpdateEntity(_EMPlayers.create(msg->entity[i].id), msg->entity[i]);
    else
      _updateEntity(_EMPlayers.create(msg->entity[i].id), msg->entity[i]);
  }
  std::unordered_set<int> currentMobsId;
  for (; i < msg->nplayer + msg->nmob; i++) {
    _updateEntity(_EMMobs.create(msg->entity[i].id), msg->entity[i]);
    currentMobsId.insert(msg->entity[i].id);
  }
  _EMMobs.removeIfNotInSet(currentMobsId);
}

void Engine::receiveMsg(MessageMap *msg) {
  _map->rcvMsg(*msg);
  if (_map->countMissingMsg() == 0 && !_map->isLoaded()) {
    if (_map->loadFromMsg()) {
      _dm.generateMapTiles(*_map);
      _state = RUNNING;
      printf("[Map] Loaded successfully\n");
    }
  }
}
