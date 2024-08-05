#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <cstdio>
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

Server::Server() : _running(true), _server(nullptr) {
  if (enet_initialize() != 0) {
    throw std::runtime_error("An error occurred while initializing ENet.");
  }
  atexit(enet_deinitialize);
}

Server::~Server() {
  std::cout << "Destroying server" << std::endl;

  deinit();
}

void Server::_loadMap(std::string const &path) {
  if (!_map.loadFromFile(path))
    throw std::runtime_error("Failed to load map");
  if (!_map.craftMsg())
    throw std::runtime_error("Failed to craft map msg");
}

void Server::init() {
  _address.host = ENET_HOST_ANY;
  _address.port = 45454;
  _server = enet_host_create(&_address, 32, 2, 0, 0);
  if (_server == nullptr) {
    throw std::runtime_error(
        "An error occurred while trying to create an ENet server host.");
  }
  std::cout << "Server initialized and listening on port " << _address.port
            << std::endl;
}

void Server::deinit() {
  if (_server != nullptr) {
    enet_host_destroy(_server);
    _server = nullptr;
  }
}

void Server::stop() {
  _running = false;
  _disconnectAllClients();
  for (auto &msg : _msgOut) {
    enet_packet_destroy(msg.packet);
    msg.packet = nullptr;
  }
}

void Server::_disconnectAllClients() {
  for (auto &user : _users) {
    enet_peer_disconnect(user.first, 0); // Send disconnect signal to client
  }
  ENetEvent event;
  while (enet_host_service(_server, &event, 3000) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_DISCONNECT:
      _handleDisconnect(event);
      break;
    default:
      break;
    }
  }
}

int Server::loadGame(std::string const &mapFile) {
  _loadMap(mapFile);
  Vector pos(64 + 16, 128 + 16);
  _hive.createMob(BASIC, pos);
  return 0;
}

void Server::run() {
  int tickRate = 120;
  ENetEvent event;
  auto lastTick = std::chrono::high_resolution_clock::now();
  auto tickDuration =
      std::chrono::milliseconds(1000 / tickRate); // Duration of each tick

  // TMP
  int tickCount = 0;

  while (true && _running) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> timeSinceLastTick =
        currentTime - lastTick;

    // TMP
    tickCount++;
    if (tickCount >= tickRate) {
      _map._resetFlow();
      for (auto const &u : _users) {
        Vector const &pos = u.second.player.getPos();
        _map._updateFlow(0, pos.x / _map.getTileSize(),
                         pos.y / _map.getTileSize());
      }
      /*_map._printFlow();*/
      tickCount = 0;
    }

    _updateGameState();
    // TMP remove the x  / 10 once line of sight is added
    if (timeSinceLastTick >= tickDuration / 10) {
      lastTick = currentTime;
      _sendGameSateToAll();
      _msgOutProcess();

      while (enet_host_service(_server, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
          _handleConnect(event);
          break;
        case ENET_EVENT_TYPE_RECEIVE:
          _handleReceive(event);
          break;
        case ENET_EVENT_TYPE_DISCONNECT:
          _handleDisconnect(event);
          break;
        default:
          std::cout << "WEIRD " << event.type << std::endl;
          break;
        }
      }
    } else {
      auto sleepTime = tickDuration - timeSinceLastTick;
      std::this_thread::sleep_for(sleepTime);
    }
  }
}

void Server::_updateGameState(void) {
  std::vector<Player *> playerVec;
  for (auto &u : _users)
    playerVec.push_back(&u.second.player);
  for (const auto &p : _hive.getMobs()) {
    const auto &mob = p.second;
    mob->findClosest(playerVec);
    Vector const &pos = mob->getPos();
    Vector aim = _map._getDirFlow(mob->getTileX(), mob->getTileY());
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

void Server::_msgOutProcess(void) {
  for (auto &msg : _msgOut) {
    if (msg.dest == ALL)
      enet_host_broadcast(_server, 0, msg.packet);
    else if (msg.dest == EXCEPT) {
      for (auto &u : _users) {
        if (u.second.getPeer() != msg.peer)
          enet_peer_send(u.second.getPeer(), 0, msg.packet);
      }
    } else if (msg.dest == SINGLE)
      enet_peer_send(msg.peer, 0, msg.packet);
  }
  _msgOut.clear();
  enet_host_flush(_server);
}

void Server::_msgOutAdd(ENetPeer *peer, ENetPacket *packet, msgDest dest) {
  t_msg msg = {peer, packet, dest};
  _msgOut.push_back(msg);
}

void Server::_sendGameSateToAll(void) {
  _craftMsgGameState();
  _msgOutAdd(0, packageMessage(_msgGameState, GAME_STATE), ALL);
}

void Server::_sendMap(ENetPeer *peer) {
  std::vector<MessageMap> const &msgMap = _map.getMsg();
  for (auto const &msg : msgMap) {
    ENetPacket *packet = packageMessage(msg, MAP);
    printf("Snd map packet %d/%d\n", msg.idPack + 1, msg.nPack);
    _msgOutAdd(peer, packet, SINGLE);
  }
}

void Server::_handleConnect(ENetEvent &event) {
  User newUser(event.peer);
  _users[event.peer] = newUser;
  newUser.player.setId(newUser.getId());
  std::cout << "A new client connected from "
            << (event.peer->address.host & 0xFF) << "."
            << ((event.peer->address.host >> 8) & 0xFF) << "."
            << ((event.peer->address.host >> 16) & 0xFF) << "."
            << ((event.peer->address.host >> 24) & 0xFF) << ":"
            << event.peer->address.port << std::endl;

  // Need to signify the ID to the nre player before broadcasting his existence
  MessagePlayerID msgId = {newUser.getId()};
  ENetPacket *packetId = packageMessage(msgId, PLR_ID);
  _msgOutAdd(event.peer, packetId, SINGLE);

  MessagePlayerCo msgCo = {newUser.getId()};
  ENetPacket *packetCo = packageMessage(msgCo, PLR_CO);
  _msgOutAdd(event.peer, packetCo, ALL);

  _sendMap(event.peer);
  printf("New player got send id %d\n", newUser.getId());
}

void Server::_handleReceive(ENetEvent &event) {
  if (event.packet->dataLength < sizeof(MessageHeader)) {
    std::cerr << "Received packet with invalid length" << std::endl;
    return;
  }
  MessageHeader *msgHeader = (MessageHeader *)event.packet->data;
  switch (msgHeader->type) {
  case PLR_UPDATE: {
    if (event.packet->dataLength !=
        sizeof(MessageHeader) + sizeof(MessagePlayerUpdate)) {
      std::cerr << "Received PLR_UPDATE packet with invalid length"
                << std::endl;
      break;
    }
    MessagePlayerUpdate *msgUpdate =
        (MessagePlayerUpdate *)(event.packet->data + sizeof(MessageHeader));
    _users[event.peer].updatePlayer(msgUpdate->angle, msgUpdate->pos,
                                    msgUpdate->vel);
    break;
  }
  default:
    break;
  }
  enet_packet_destroy(event.packet);
}

void Server::_handleDisconnect(ENetEvent &event) {
  int userId = _users[event.peer].getId();
  std::cout << "Player " << userId << " disconnected." << std::endl;
  _users.erase(event.peer);

  MessagePlayerDisco msg = {userId};
  ENetPacket *packet = packageMessage(msg, PLR_DISCO);
  _msgOutAdd(nullptr, packet, ALL);
}

void Server::_distributeToOther(ENetPeer *author, ENetPacket *packet) {
  for (const auto &u : _users) {
    if (u.first != author)
      u.second.send(packet);
  }
}

void Server::_distributeToAll(ENetPacket *packet) {
  for (const auto &u : _users)
    u.second.send(packet);
}

void Server::_craftMsgGameState(void) {
  _msgGameState.nplayer = _users.size();
  int i = 0;
  for (const auto &u : _users) {
    if (i >= MAX_N_PLAYER) {
      std::cout << _msgGameState.nplayer << ": too much players !" << std::endl;
      break;
    }
    _msgGameState.players[i].angle = u.second.player.getAngle();
    _msgGameState.players[i].vel = u.second.player.getVel();
    _msgGameState.players[i].pos = u.second.player.getPos();
    _msgGameState.players[i].id = u.second.getId();
    i++;
  }
  _hive.craftMobMsgState(_msgGameState);
}
