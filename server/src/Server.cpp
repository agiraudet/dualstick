#include <bits/chrono.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <enet/enet.h>
#include <iostream>
#include <ratio>
#include <stdexcept>
#include <thread>

#include "Entity.hpp"
#include "Message.hpp"
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
  _hive.createMob(BASIC, pos);
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
  constexpr int tickRate = 120;
  constexpr auto tickDuration = std::chrono::milliseconds(1000 / tickRate);
  auto lastTick = std::chrono::high_resolution_clock::now();

  while (_running) {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> timeSinceLastTick =
        currentTime - lastTick;
    _updateGameState();
    _listener.recv(*this);
    if (timeSinceLastTick >= tickDuration) {
      lastTick = currentTime;
      _sendGameSateToAll();
      _listener.send();
    } else {
      std::this_thread::sleep_for(tickDuration - timeSinceLastTick);
    }
  }
}

void Server::_updateGameState(void) {
  for (const auto &p : _hive.getMobs()) {
    const auto &mob = p.second;
    mob->findClosest(_playersPtr);
    Vector const &pos = mob->getPos();
    Vector aim = _flow.getDir(mob->getTileX(), mob->getTileY());
    aim.x =
        (mob->getTileX() + aim.x) * _map.getTileSize() + _map.getTileSize() / 2;
    aim.y =
        (mob->getTileY() + aim.y) * _map.getTileSize() + _map.getTileSize() / 2;
    Vector dir = aim - pos;
    dir = dir.normalize();
    dir = dir * ENTITY_MAXSPEED;
    mob->setVel(dir);
    mob->capSpeed();
    mob->move();
  }
}

void Server::_sendGameSateToAll(void) {
  _craftMsgGameState();
  _listener.msgOutAdd(-1, packageMessage(_msgGameState, GAME_STATE), ALL);
}

void Server::_sendMap(int id) {
  std::vector<MessageMap> const &msgMap = _map.getMsg();
  for (auto const &msg : msgMap) {
    ENetPacket *packet = packageMessage(msg, MAP);
    printf("Snd map packet %d/%d\n", msg.idPack + 1, msg.nPack);
    _listener.msgOutAdd(id, packet, SINGLE);
  }
}

void Server::_craftMsgGameState(void) {
  _msgGameState.nplayer = _players.size();
  int i = 0;
  for (const auto &u : _players) {
    if (i >= MAX_N_PLAYER) {
      std::cout << _msgGameState.nplayer << ": too much players !" << std::endl;
      break;
    }
    _msgGameState.players[i].angle = u.second.getAngle();
    _msgGameState.players[i].vel = u.second.getVel();
    _msgGameState.players[i].pos = u.second.getPos();
    _msgGameState.players[i].id = u.second.getId();
    i++;
  }
  _hive.craftMobMsgState(_msgGameState);
}

void Server::_refreshPlayerPtr(void) {
  _playersPtr.clear();
  for (auto &p : _players)
    _playersPtr.push_back(&p.second);
  _flow.updatePlayerVec(&_playersPtr);
}

void Server::playerAdd(int id) {
  _players[id] = Player();
  _players[id].setId(id);
  _sendMap(id);
  _refreshPlayerPtr();
}

void Server::playerRemove(int id) {
  auto it = _players.find(id);
  if (it != _players.end()) {
    _players.erase(it);
    _refreshPlayerPtr();
  }
}

void Server::playerUpdate(int id, Vector &pos, Vector &vel, float angle) {
  auto it = _players.find(id);
  if (it != _players.end()) {
    it->second.setPos(pos);
    it->second.setVel(vel);
    it->second.setAngle(angle);
  }
}

void Server::playerShoot(int id) {
  auto it = _players.find(id);
  if (it != _players.end()) {
    _map.rayCast(it->second, _hive.getMobs());
  }
}
