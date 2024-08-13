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

#include "Message.hpp"
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

Server::Server() : _status(LOADING), _flow(_map) {
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
  _map.loadShopsWIP();
  if (!_map.craftMsg())
    throw std::runtime_error("Failed to craft map msg");
}

int Server::loadGame(std::string const &mapFile) {
  _loadMap(mapFile);
  _flow.init();
  _wm.addSpawner(_EMMob, Vector(20 * 32 + 16, 4 * 32 + 16));
  _wm.addSpawner(_EMMob, Vector(5 * 32 + 16, 28 * 32 + 16));
  _wm.addSpawner(_EMMob, Vector(26 * 32 + 16, 21 * 32 + 16));
  _wm.setDelayAll(std::chrono::milliseconds(5000));
  _wm.setRunAll(true);
  return 0;
}

void Server::start() {
  if (!_listener.connect())
    exit(EXIT_FAILURE);
  _status = READY;
  _flow.startUpdating();
  _run();
}

void Server::stop() {
  _flow.stopUpdating();
  _status = STOPPED;
}

void Server::_run() {
  constexpr int tickRate = 60;
  constexpr auto tickDuration = std::chrono::milliseconds(1000 / tickRate);
  auto lastTick = std::chrono::high_resolution_clock::now();

  while (_status == RUNNING || _status == READY) {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> timeSinceLastTick =
        currentTime - lastTick;
    if (_status == RUNNING) {
      _updateGameState();
    }
    _listener.recv(*this);
    _listener.send();
    if (timeSinceLastTick >= tickDuration) {
      lastTick = currentTime;
      if (_status == RUNNING)
        _sendGameSateToAll();
    } else {
      std::this_thread::sleep_for(tickDuration - timeSinceLastTick);
    }
  }
}

void Server::_updateGameState(void) {
  _EMMob.removeDeads();
  if (_wm.allSpwanersEmpty() && _EMMob.empty())
    _wm.nextWave();
  _wm.process();
  if (!_EMPlayer.empty()) {
    for (auto &[id, mob] : _EMMob) {
      auto target = mob->findClosest(_EMPlayer);
      // if (target && _map.lineOfSight(*mob, *target)) {
      Vector leftEye = mob->getPos() - mob->getSize() / 2;
      Vector rightEye = mob->getPos() + mob->getSize() / 2;
      if (target && _map.lineOfSight(leftEye, mob->getAngle(), *target) &&
          _map.lineOfSight(rightEye, mob->getAngle(), *target)) {
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
  auto msgMapShop = _map.getMsgShop();
  for (auto const &msg : msgMapShop) {
    ENetPacket *packet = packageMessage(msg, MAP_SHOP, true);
    _listener.msgOutAdd(id, packet, SINGLE);
  }
}

void Server::_craftMsgGameState(void) {
  static uint32_t stamp = 0;
  _msgGameState.stamp = stamp++;
  _msgGameState.nwave = _wm.getWave();
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
  if (_status == READY)
    _status = RUNNING;
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
  if (_EMPlayer.empty())
    _status = READY;
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
        if (!hit->isAlive()) {
          player->addMoney(10);
          MessagePlayerMoney msg = {id, player->getMoney()};
          _listener.msgOutAdd(id, packageMessage(msg, PLR_MONEY, true), SINGLE);
        }
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

void Server::playerBuy(int id) {
  auto player = _EMPlayer.get(id);
  if (player)
    _map.playerInterract(*player);
}
