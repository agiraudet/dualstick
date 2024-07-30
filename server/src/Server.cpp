#include <algorithm>
#include <enet/enet.h>
#include <iostream>
#include <stdexcept>

#include "Message.hpp"
#include "Server.hpp"

Server::Server() : _server(nullptr) {
  if (enet_initialize() != 0) {
    throw std::runtime_error("An error occurred while initializing ENet.");
  }
  atexit(enet_deinitialize);
}

Server::~Server() { deinit(); }

void Server::init() {
  _address.host = ENET_HOST_ANY;
  _address.port = 1234; // Server will bind to this port
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

void Server::run() {
  ENetEvent event;
  while (true) {
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
    _msgOutClean();
  }
}

void Server::_msgOutProcess(void) {
  for (auto &msg : _msgOut) {
    if (msg.peer == nullptr)
      _distributeToAll(msg.packet);
    else if (msg.ccAll)
      _distributeToOther(msg.peer, msg.packet);
    else
      _users[msg.peer].send(msg.packet);
    msg.processed = true;
  }
}

void Server::_msgOutClean(void) {
  auto should_remove = [](t_msg &msg) {
    if (msg.processed) {
      /*enet_packet_destroy(msg.packet);*/
      return true;
    }
    return false;
  };
  _msgOut.erase(std::remove_if(_msgOut.begin(), _msgOut.end(), should_remove),
                _msgOut.end());
}

void Server::_msgOutAdd(ENetPeer *peer, ENetPacket *packet, bool ccAll) {
  t_msg msg = {peer, packet, ccAll, false};
  _msgOut.push_back(msg);
}

void Server::_sendGameSateToAll(void) {
  _msgOutAdd(nullptr, packageMessage(_craftMsgGameState(), GAME_STATE), true);
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

  MessagePlayerCo msgCo = {newUser.getId()};
  ENetPacket *packetCo = packageMessage(msgCo, PLR_CO);
  _msgOutAdd(event.peer, packetCo, true);

  MessagePlayerID msgId = {newUser.getId()};
  ENetPacket *packetId = packageMessage(msgId, PLR_ID);
  _msgOutAdd(event.peer, packetId, false);

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
    _users[event.peer].updatePlayer(msgUpdate->pos, msgUpdate->vel);
    break;
  }
  default:
    break;
  }
  enet_packet_destroy(event.packet);
}

void Server::_handleDisconnect(ENetEvent &event) {
  std::cout << "Client disconnected." << std::endl;

  int userId = _users[event.peer].getId();
  _users.erase(event.peer);

  MessagePlayerDisco msg = {userId};
  ENetPacket *packet = packageMessage(msg, PLR_DISCO);
  _msgOutAdd(nullptr, packet, true);
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

MessageGameState Server::_craftMsgGameState(void) {
  MessageGameState msg;
  msg.nplayer = _users.size();
  int i = 0;
  for (const auto &u : _users) {
    if (i >= MAX_N_PLAYER) {
      std::cout << msg.nplayer << ": too much players !";
      break;
    }
    msg.players[i].vel = u.second.player.getVel();
    msg.players[i].pos = u.second.player.getPos();
    msg.players[i].id = u.second.getId();
    i++;
  }
  return msg;
}
