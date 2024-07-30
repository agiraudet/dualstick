#include "Engine.hpp"
#include "Message.hpp"
#include "Timer.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <cstdio>
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
/**/
/*void Engine::run(void) {*/
/*  TimerFps timerFps(30);*/
/*  MessagePlayerUpdate msg{-1, Vector(-99, -99), Vector(-99, -99)};*/
/*  Uint32 lastUpdate = SDL_GetTicks();*/
/*  Uint32 currentTime = lastUpdate;*/
/**/
/*  while (_alive) {*/
/*    _client.getEvent(*this);*/
/*    _getEvent();*/
/*    _player.applyInput();*/
/**/
/*    // fct*/
/*    currentTime = SDL_GetTicks();*/
/*    if (currentTime - lastUpdate > 10) {*/
/*      lastUpdate = currentTime;*/
/*      bool modidified = false;*/
/*      msg.id = _player.getId();*/
/*      Vector const &plrPos = _player.getPos();*/
/*      Vector const &plrVel = _player.getVel();*/
/*      if (msg.pos != plrPos) {*/
/*        modidified = true;*/
/*        msg.pos = plrPos;*/
/*      }*/
/*      if (msg.vel != plrVel) {*/
/*        modidified = true;*/
/*        msg.vel = plrVel;*/
/*      }*/
/*      if (modidified) {*/
/*        ENetPacket *pck = packageMessage(msg, PLR_UPDATE);*/
/*        _client.sendMessage(pck);*/
/*        msg.vel = _player.getVel();*/
/*      }*/
/*    }*/
/*    // end fct*/
/**/
/*    _render();*/
/*  }*/
/*}*/

void Engine::run(void) {
  MessagePlayerUpdate msg{-1, Vector(-99, -99), Vector(-99, -99)};
  Uint32 lastUpdate = SDL_GetTicks();
  Uint32 currentTime = lastUpdate;
  Uint32 frameCount = 0;
  Uint32 startTime = lastUpdate;
  const Uint32 fpsUpdateInterval =
      1000; // Update FPS every 1000 milliseconds (1 second)
  const Uint32 targetFrameTime =
      1000 / 60; // Target frame time in milliseconds for 30 FPS

  while (_alive) {
    _client.getEvent(*this);
    _getEvent();
    _player.applyInput();

    // fct
    currentTime = SDL_GetTicks();
    /*if (currentTime - lastUpdate > 10) {*/
    if (true) {
      /*lastUpdate = currentTime;*/
      bool modified = false;
      msg.id = _player.getId();
      Vector const &plrPos = _player.getPos();
      Vector const &plrVel = _player.getVel();
      if (msg.pos != plrPos) {
        modified = true;
        msg.pos = plrPos;
      }
      if (msg.vel != plrVel) {
        modified = true;
        msg.vel = plrVel;
      }
      if (modified) {
        ENetPacket *pck = packageMessage(msg, PLR_UPDATE);
        _client.sendMessage(pck);
        msg.vel = _player.getVel();
      }
    }
    // end fct

    _render();
    frameCount++;

    // Calculate and print FPS
    Uint32 elapsedTime = currentTime - startTime;
    if (elapsedTime >= fpsUpdateInterval) {
      float fps = frameCount / (elapsedTime / 1000.0f);
      std::cout << "Average FPS: " << fps << std::endl;
      startTime = currentTime;
      frameCount = 0;
    }

    // Cap the frame rate to 30 FPS
    Uint32 frameTime = SDL_GetTicks() - currentTime;
    if (frameTime < targetFrameTime) {
      SDL_Delay(targetFrameTime - frameTime);
    }
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

void Engine::updatePlayer(int id, Vector &pos, Vector &vel) {
  /*printf("Update player id %d\n", id);*/
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
