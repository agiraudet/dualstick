#include "Engine.hpp"
#include "Atlas.hpp"
#include "MapRend.hpp"
#include "Timer.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <cstdio>
#include <cstdlib>
#include <enet/enet.h>
#include <enet/types.h>
#include <exception>
#include <iostream>

Engine::Engine(void)
    : _client(Client()), _state(LOADING),
      _msgPlayerUpdate({-1, 0.f, Vector(-99, -99), Vector(-99, -99)}),
      _window(nullptr), _renderer(nullptr), _atlas(nullptr),
      _camera({0, 0, SCR_WIDTH, SCR_HEIGHT}) {
  SDL_Init(SDL_INIT_VIDEO);
  _window = SDL_CreateWindow("Client", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT,
                             SDL_WINDOW_SHOWN);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  _atlas = new Atlas(_renderer);
  _fillAtlas();
  _map = new MapRend(_atlas, "tiles");
  Vector initPos(320.f, 320.f);
  _player.setPos(initPos);
}

Engine::~Engine(void) {
  delete _atlas;
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void Engine::_fillAtlas(void) {
  _atlas->loadTexture("other", "../assets/other.png");
  _atlas->loadTexture("player", "../assets/player.png");
  _atlas->loadTexture("tiles", "../assets/tiles.png", 80, 80);
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

void Engine::receivedMap(MessageMap &msg) {
  _map->rcvMsg(msg);
  if (_map->countMissingMsg() == 0 && !_map->isLoaded()) {
    if (_map->loadFromMsg()) {
      _map->createTiles();
      _state = RUNNING;
      printf("Map: loaded successfully\n");
    }
  }
}

void Engine::_centerCameraOnPlayer(void) {
  _camera.x = (_player.getPos().x + 25) - (float)SCR_WIDTH / 2.f;
  _camera.y = (_player.getPos().y + 25) - (float)SCR_HEIGHT / 2.f;
}

void Engine::run(void) {
  TimerFps fpsTimer(60.f);
  while (_state != STOPPED) {
    _client.getEvent(*this);
    _getEvent();
    if (_state == RUNNING) {
      _player.applyInput();
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
    ENetPacket *pck = packageMessage(_msgPlayerUpdate, PLR_UPDATE);
    _client.sendMessage(pck);
  }
}

void Engine::setPlayerId(int id) { _player.setId(id); }

void Engine::removePlayer(int id) {
  if (_otherPlayers.find(id) != _otherPlayers.end()) {
    _otherPlayers.erase(id);
  }
}

void Engine::addPlayer(int id) {
  if (_otherPlayers.find(id) != _otherPlayers.end()) {
    _otherPlayers.erase(id);
  }
  _otherPlayers[id] = Player(id);
}

void Engine::updatePlayer(MessagePlayerUpdate *msg) {
  if (!msg || msg->id == _player.getId())
    return;
  std::unordered_map<int, Player>::iterator it = _otherPlayers.find(msg->id);
  if (it == _otherPlayers.end() && msg->id != _player.getId())
    addPlayer(msg->id);
  _otherPlayers[msg->id].setPos(msg->pos);
  _otherPlayers[msg->id].setVel(msg->vel);
  _otherPlayers[msg->id].setAngle(msg->angle);
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

void Engine::_render(void) {
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  SDL_RenderClear(_renderer);

  _map->render(_camera);

  _atlas->drawTexture("player", _player.getPos().x - 25 - _camera.x,
                      _player.getPos().y - 25 - _camera.y, _player.getAngle());
  for (const auto &p : _otherPlayers) {
    _atlas->drawTexture("other", p.second.getPos().x - 25 - _camera.x,
                        p.second.getPos().y - 25 - _camera.y,
                        p.second.getAngle());
  }
  SDL_RenderPresent(_renderer);
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
  }
}
