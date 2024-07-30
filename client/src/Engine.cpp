#include "Engine.hpp"
#include "Message.hpp"
#include "Timer.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <enet/enet.h>
#include <iostream>

Engine::Engine(void)
    : _client(Client()), _alive(true), _window(nullptr), _renderer(nullptr) {
  _client.init("localhost", 1234);
  SDL_Init(SDL_INIT_VIDEO);
  _window = SDL_CreateWindow("Client", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT,
                             SDL_WINDOW_SHOWN);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
}

Engine::~Engine(void) {
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void Engine::run(void) {
  TimerFps timerFps(30);

  while (_alive) {
    _client.getEvent(*this);
    _getEvent();
    _player.applyInput();
    MessagePlayerUpdate msg = {_player.getId(), _player.getPos(),
                               _player.getVel()};
    ENetPacket *pck = packageMessage(msg, PLR_UPDATE);
    _client.sendMessage(pck);
    /*enet_packet_destroy(pck);*/
    _render();
    timerFps.capFps();
  }
}

void Engine::setPlayerId(int id) { _player.setId(id); }

void Engine::removePlayer(int id) {
  if (_otherPlayers.find(id) != _otherPlayers.end()) {
    _otherPlayers.erase(id);
  }
  std::cout << "remove player " << id << std::endl;
}

void Engine::addPlayer(int id) {
  if (_otherPlayers.find(id) != _otherPlayers.end()) {
    _otherPlayers.erase(id);
  }
  _otherPlayers[id] = Player(id);
  std::cout << "added player " << id << std::endl;
}

void Engine::updatePlayer(int id, Vector &pos, Vector &vel) {
  std::cout << "update player " << id << std::endl;
  std::unordered_map<int, Player>::iterator it = _otherPlayers.find(id);
  if (it == _otherPlayers.end() && id != _player.getId())
    addPlayer(id);
  it = _otherPlayers.find(id);
  if (it != _otherPlayers.end()) {
    _otherPlayers[id].setPos(pos);
    _otherPlayers[id].setVel(vel);
  }
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

  SDL_Rect rect = {(int)_player.getPos().x, (int)_player.getPos().y, 50, 50};
  SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(_renderer, &rect);

  SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 255);
  for (const auto &p : _otherPlayers) {
    rect.x = p.second.getPos().x;
    rect.y = p.second.getPos().y;
    SDL_RenderFillRect(_renderer, &rect);
  }

  SDL_RenderPresent(_renderer);
}

void Engine::_processInput(SDL_Keycode &sym, bool state) {
  switch (sym) {
  case SDLK_UP:
    _player.setInput(UP, state);
    break;
  case SDLK_DOWN:
    _player.setInput(DOWN, state);
    break;
  case SDLK_LEFT:
    _player.setInput(LEFT, state);
    break;
  case SDLK_RIGHT:
    _player.setInput(RIGHT, state);
    break;
  }
}
