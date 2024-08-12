#include <bits/chrono.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <enet/enet.h>
#include <iostream>
#include <iterator>
#include <ratio>
#include <stdexcept>
#include <thread>

#include "Mob.hpp"
#include "Player.hpp"
#include "Server.hpp"
#include "Vector.hpp"
#include "WaveManager.hpp"

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
  WaveManager::WM().addSpawner(_EMMob, Vector(20 * 32 + 16, 4 * 32 + 16));
  WaveManager::WM().newWave(9);
  WaveManager::WM().setDelayAll(std::chrono::milliseconds(5000));
  WaveManager::WM().setRunAll(true);
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
  _EMMob.removeDeads();
  WaveManager::WM().process();
  if (!_EMPlayer.empty()) {
    for (auto &[id, mob] : _EMMob) {
      auto target = mob->findClosest(_EMPlayer);
      if (target && _map.lineOfSight(*mob, *target)) {
        mob->setVelTowards(target->getPos());
      } else {
        mob->processDir(_flow.getDir(mob->getTileX(), mob->getTileY()),
                        _map.getTileSize());
      }
      mob->move(_map);
      if (mob->tryHitTarget()) {
        MessageMobAttack msg = {id, mob->getTarget()->getId()};
        _listener.msgOutAdd(-1, packageMessage(msg, MOB_ATTACK, true), ALL);
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
  _flow.updatePlayerVec(_EMPlayer);
  _sendMap(newPlayerId);
  return newPlayerId;
}

int Server::playerAdd(int id) {
  Vector pos(64, 64);
  _EMPlayer.create(id)->setPos(pos);
  _flow.updatePlayerVec(_EMPlayer);
  _sendMap(id);
  return id;
}

void Server::playerRemove(int id) {
  _EMPlayer.remove(id);
  _flow.updatePlayerVec(_EMPlayer);
  std::cout << "[Server] Removed player " << id << std::endl;
  std::cout << "[Server] Players remaining: " << _EMPlayer.size() << std::endl;
}

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
    if (player->weapon && player->weapon->fire()) {
      auto hit =
          _map.rayCast(*player, _EMMob, player->weapon->range, hitX, hitY);
      if (hit) {
        MessageMobHit msg = {hit->getId(), hitX, hitY};
        _listener.msgOutAdd(id, packageMessage(msg, MOB_HIT, true), ALL);
      }
    }
  }
}

void Server::playerReload(int id) {
  auto player = _EMPlayer.get(id);
  if (player && player->weapon)
    player->weapon->reload();
}
