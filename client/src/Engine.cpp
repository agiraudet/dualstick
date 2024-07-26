#include "Engine.hpp"
#include <SDL2/SDL_keycode.h>

Engine::Engine(void) : _alive(true), _window(nullptr), _renderer(nullptr) {
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
  while (_alive) {
    _getEvent();
    _render();
  }
}

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

void Engine::_getEvent(void) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      _alive = false;
    else if (e.type == SDL_KEYDOWN)
      _processInput(e.key.keysym.sym);
  }
}

void Engine::_render(void) {
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  SDL_RenderClear(_renderer);

  SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(_renderer, &_player.getRect());

  SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 255);
  for (const auto &p : _otherPlayers) {
    SDL_RenderFillRect(_renderer, &p.second.getRect());
  }

  SDL_RenderPresent(_renderer);
}

void Engine::_processInput(SDL_Keycode &sym) {
  int dx = 0;
  int dy = 0;
  switch (sym) {
  case SDLK_UP:
    dy = -10;
    break;
  case SDLK_DOWN:
    dy = 10;
    break;
  case SDLK_LEFT:
    dx = -10;
    break;
  case SDLK_RIGHT:
    dx = 10;
    break;
  }
  _player.move(dx, dy);
}
