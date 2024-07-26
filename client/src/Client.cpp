#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <stdexcept>
#include <x86gprintrin.h>

#include "Client.hpp"
#include "Engine.hpp"
#include "Message.hpp"

Client::Client() : _client(nullptr), _peer(nullptr) {
  if (enet_initialize() != 0) {
    throw std::runtime_error("An error occurred while initializing ENet.");
  }
  atexit(enet_deinitialize);
}

Client::~Client() {
  disconnect();
  deinit();
}

void Client::init(const std::string &host, enet_uint16 port) {
  _client = enet_host_create(nullptr, 1, 2, 0, 0);
  if (_client == nullptr) {
    throw std::runtime_error(
        "An error occurred while trying to create an ENet _client host.");
  }

  ENetAddress address;
  enet_address_set_host(&address, host.c_str());
  address.port = port;

  _peer = enet_host_connect(_client, &address, 2, 0);
  if (_peer == nullptr) {
    throw std::runtime_error(
        "No available _peers for initiating an ENet connection.");
  }

  ENetEvent event;
  if (enet_host_service(_client, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    std::cout << "Connection to " << host << ":" << port << " succeeded."
              << std::endl;
  } else {
    enet_peer_reset(_peer);
    throw std::runtime_error("Connection to " + host + ":" +
                             std::to_string(port) + " failed.");
  }
}

void Client::deinit() {
  if (_client != nullptr) {
    enet_host_destroy(_client);
    _client = nullptr;
    _peer = nullptr;
  }
}

void Client::disconnect(void) {
  ENetEvent event;
  enet_peer_disconnect(_peer, 0);
  while (enet_host_service(_client, &event, 3000) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_RECEIVE:
      enet_packet_destroy(event.packet);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      puts("Disconnection succeeded.");
      break;
    default:
      break;
    }
  }
}

void Client::getEvent(Engine &eng) {
  ENetEvent event;
  while (enet_host_service(_client, &event, 0) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_RECEIVE:
      _handleReceive(event, eng);
      enet_packet_destroy(event.packet);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      std::cout << "Disconnected from server." << std::endl;
      return;
    default:
      break;
    }
  }
}

void Client::sendMessage(const std::string &message) {
  if (_peer != nullptr) {
    ENetPacket *packet = enet_packet_create(
        message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(_peer, 0, packet);
  }
}

void Client::sendMessage(ENetPacket *packet) {
  if (_peer != nullptr) {
    enet_peer_send(_peer, 0, packet);
  }
  /*enet_host_flush(_client);*/
}

void Client::_handleReceive(ENetEvent &event, Engine &eng) {
  std::cout << "A packet of length " << event.packet->dataLength
            << " containing \"" << event.packet->data << "\" was received."
            << std::endl;
  MessageHeader msgHeader;
  memcpy(&msgHeader, event.packet->data, sizeof(MessageHeader));
  std::cout << "msg Type: " << msgHeader.type << std::endl;
  switch (msgHeader.type) {
  case PLR_CO: {
    MessagePlayerCo msgCo =
        deserializeMessage<MessagePlayerCo>((const char *)event.packet->data);
    eng.addPlayer(msgCo.id);
    break;
  }
  case PLR_DISCO: {
    MessagePlayerDisco msgDisco = deserializeMessage<MessagePlayerDisco>(
        (const char *)event.packet->data);
    eng.removePlayer(msgDisco.id);
    break;
  }
  case PLR_UPDATE: {
    MessagePlayerUpdate msgUpdate = deserializeMessage<MessagePlayerUpdate>(
        (const char *)event.packet->data);
    eng.updatePlayer(msgUpdate.id, msgUpdate.rect);
    break;
  }
  case PLR_ID: {
    MessagePlayerID msgID =
        deserializeMessage<MessagePlayerID>((const char *)event.packet->data);
    eng.setPlayerId(msgID.id);
    break;
  }
  default:
    break;
  }
}
