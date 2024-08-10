#include <bits/chrono.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <enet/enet.h>
#include <iostream>
#include <ratio>
#include <stdexcept>
#include <thread>

#include "Mob.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "Vector.hpp"

Server *g_serverInstance = nullptr;

void signalHandler(int signum) {
  (void)signum;
  if (g_serverInstance) {
    std::cout << "Stoping..." << std::endl;
    g_serverInstance->stop();
  }
}

Server::Server() : _running(false), _flow(_map) {
  if (enet_initialize() != 0) {
    throw std::runtime_error("An error occurred while initializing ENet.");
  }
  atexit(enet_deinitialize);
}

Server::~Server() {
  std::cout << "Destroying server" << std::endl;
  if (_listener.connected())
    _listener.disconnect();
}

void Server::_loadMap(std::string const &path) {
  if (!_map.loadFromFile(path))
    throw std::runtime_error("Failed to load map");
  if (!_map.craftMsg())
    throw std::runtime_error("Failed to craft map msg");
}

int Server::loadGame(std::string const &mapFile) {
  _loadMap(mapFile);
  _flow.init();

  Vector pos(64 + 16, 128 + 16);
  Vector pos2(32 * 22, 32 * 24);
  Vector pos3(32 * 7, 32 * 7);
  _EMMob.get(_EMMob.create())->setPos(pos3);
  _EMMob.get(_EMMob.create())->setPos(pos2);
  _EMMob.get(_EMMob.create())->setPos(pos);
  return 0;
}

void Server::start() {
  if (!_listener.connect())
    exit(EXIT_FAILURE);
  _running = true;
  _flow.startUpdating();
  _run();
}

void Server::stop() {
  _flow.stopUpdating();
  _running = false;
}

void Server::_run() {
  constexpr int tickRate = 60;
  constexpr auto tickDuration = std::chrono::milliseconds(1000 / tickRate);
  auto lastTick = std::chrono::high_resolution_clock::now();

  while (_running) {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> timeSinceLastTick =
        currentTime - lastTick;
    _updateGameState();
    _listener.recv(*this);
    _listener.send();
    if (timeSinceLastTick >= tickDuration) {
      lastTick = currentTime;
      _sendGameSateToAll();
    } else {
      std::this_thread::sleep_for(tickDuration - timeSinceLastTick);
    }
  }
}

void Server::_updateGameState(void) {
  /*std::this_thread::sleep_for(std::chrono::duration<double,
   * std::milli>(1000));*/
  _EMMob.removeDeads();
  if (!_EMPlayer.empty()) {
    for (auto &[id, mob] : _EMMob) {
      auto target = mob->findClosest(_EMPlayer);
      Vector const &pos = mob->getPos();
      Vector aim = _flow.getDir(mob->getTileX(), mob->getTileY());
      if (aim == Vector(0, 0))
        continue;
      aim.x = (mob->getTileX() + aim.x) * _map.getTileSize() +
              (float)_map.getTileSize() / 2;
      aim.y = (mob->getTileY() + aim.y) * _map.getTileSize() +
              (float)_map.getTileSize() / 2;
      Vector dir = aim - pos;
      dir = dir.normalize();
      dir = dir * 80;
      mob->setVel(dir);
      /*mob->capSpeed();*/
      mob->move();
      if (target && mob->weapon) {
        if (mob->getPos().distance(target->getPos()) < mob->weapon->range &&
            mob->weapon->fire()) {
          mob->weapon->dealDamage(*target, 0.f);
          MessageMobAttack msg = {id, target->getId()};
          _listener.msgOutAdd(-1, packageMessage(msg, MOB_ATTACK, true), ALL);
        }
      }
    }
    _flow.updatePlayerVec(_EMPlayer);
  }
}

void Server::_sendGameSateToAll(void) {
  _craftMsgGameState();
  _listener.msgOutAdd(-1, packageMessage(_msgGameState, GAME_STATE, false),
                      ALL);
}

void Server::_sendMap(int id) {
  std::vector<MessageMap> const &msgMap = _map.getMsg();
  for (auto const &msg : msgMap) {
    ENetPacket *packet = packageMessage(msg, MAP, true);
    printf("[Map] Sent packet %d/%d\n", msg.idPack + 1, msg.nPack);
    _listener.msgOutAdd(id, packet, SINGLE);
  }
}

void Server::_craftMsgGameState(void) {
  static uint32_t stamp = 0;
  _msgGameState.stamp = stamp++;
  int nPlayer = MAX_N_PLAYER;
  _EMPlayer.fillMsg(nPlayer, _msgGameState.entity);
  _msgGameState.nplayer = nPlayer;
  int nMob = MAX_N_PLAYER + MAX_N_MOB - nPlayer;
  _EMMob.fillMsg(nMob, &_msgGameState.entity[nPlayer]);
  _msgGameState.nmob = nMob;
}

int Server::playerAdd(void) {
  int newPlayerId = _EMPlayer.create();
  _sendMap(newPlayerId);
  return newPlayerId;
}

int Server::playerAdd(int id) {
  Vector pos(64, 64);
  _EMPlayer.create(id)->setPos(pos);
  _sendMap(id);
  return id;
}

void Server::playerRemove(int id) { _EMPlayer.remove(id); }

void Server::playerUpdate(int id, Vector &pos, Vector &vel, float angle) {
  auto player = _EMPlayer.get(id);
  if (player)
    player->update(pos, vel, angle, player->getHp());
}

void Server::playerShoot(int id) {
  auto player = _EMPlayer.get(id);
  if (player) {
    int hitX = 0;
    int hitY = 0;
    int hit = _map.rayCast(*player, _EMMob, hitX, hitY);
    if (hit != -1) {
      MessageMobHit msg = {hit, hitX, hitY};
      _listener.msgOutAdd(id, packageMessage(msg, MOB_HIT, true), ALL);
    }
  }
}
