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

Server::~Server() {
  if (_server != nullptr) {
    enet_host_destroy(_server);
  }
}

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
    while (enet_host_service(_server, &event, 1000) > 0) {
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
        break;
      }
    }
  }
}

void Server::_handleConnect(ENetEvent &event) {
  User newUser(event.peer);
  _users[event.peer] = newUser;
  std::cout << "A new client connected from "
            << (event.peer->address.host & 0xFF) << "."
            << ((event.peer->address.host >> 8) & 0xFF) << "."
            << ((event.peer->address.host >> 16) & 0xFF) << "."
            << ((event.peer->address.host >> 24) & 0xFF) << ":"
            << event.peer->address.port << std::endl;
  MessagePlayerCo msg = {newUser.getId()};
  ENetPacket *packet = packageMessage(msg, PLR_CO);
  _distributeToOther(newUser.getId(), packet);
  enet_packet_destroy(packet);
}

void Server::_handleReceive(ENetEvent &event) {
  std::cout << "A packet of length " << event.packet->dataLength
            << " containing \"" << event.packet->data << "\" was received from "
            << (char *)event.peer->data << std::endl;
  ENetPacket *pck = enet_packet_create(
      event.packet->data, event.packet->dataLength, ENET_PACKET_FLAG_RELIABLE);
  _distributeToOther(_users[event.peer].getId(), pck);
  enet_packet_destroy(event.packet);
  enet_packet_destroy(pck);
  /*if (_users.find(event.peer) != _users.end()) {*/
  /*  _users[event.peer].send("Message received");*/
  /*}*/
}

void Server::_handleDisconnect(ENetEvent &event) {
  std::cout << "Client disconnected." << std::endl;
  int userId = _users[event.peer].getId();
  _users.erase(event.peer);
  MessagePlayerDisco msg = {userId};
  ENetPacket *packet = packageMessage(msg, PLR_DISCO);
  _distributeToAll(packet);
  enet_packet_destroy(packet);
}

void Server::_distributeToOther(int authorId, ENetPacket *packet) {
  for (const auto &u : _users) {
    if (u.second.getId() != authorId)
      u.second.send(packet);
  }
}

void Server::_distributeToAll(ENetPacket *packet) {
  for (const auto &u : _users)
    u.second.send(packet);
}
