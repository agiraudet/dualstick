#include "Engine.hpp"
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
    : _client(Client()), _alive(true),
      _msgPlayerUpdate({-1, 0.f, Vector(-99, -99), Vector(-99, -99)}),
      _window(nullptr), _renderer(nullptr) {
  SDL_Init(SDL_INIT_VIDEO);
  _window = SDL_CreateWindow("Client", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT,
                             SDL_WINDOW_SHOWN);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  // TMP
  _texPlayer = IMG_LoadTexture(_renderer, "../assets/player.png");
  _texOther = IMG_LoadTexture(_renderer, "../assets/other.png");
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
  while (_alive) {
    _client.getEvent(*this);
    _getEvent();
    _player.applyInput();
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    _player.aimAngle(mouseX, mouseY);
    _NotifyPlayerUpdate();
    _render();
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
      _alive = false;
    else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
      _processInput(e.key.keysym.sym, (e.type == SDL_KEYDOWN));
  }
}

void Engine::_render(void) {
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  SDL_RenderClear(_renderer);

  SDL_Rect rect = {(int)_player.getPos().x - 25, (int)_player.getPos().y - 25,
                   50, 50};
  SDL_Point center = {rect.w / 2, rect.h / 2};
  SDL_RenderCopyEx(_renderer, _texPlayer, NULL, &rect, _player.getAngle(),
                   &center, SDL_FLIP_NONE);

  for (const auto &p : _otherPlayers) {
    rect.x = p.second.getPos().x - 25;
    rect.y = p.second.getPos().y - 25;
    center.x = rect.w / 2;
    center.y = rect.h / 2;
    SDL_RenderCopyEx(_renderer, _texOther, NULL, &rect, p.second.getAngle(),
                     &center, SDL_FLIP_NONE);
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
