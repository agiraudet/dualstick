#include <cstdio>
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <stdexcept>

#include "Client.hpp"
#include "Engine.hpp"
#include "Message.hpp"

Client::Client() : _client(nullptr), _peer(nullptr) {
  if (enet_initialize() != 0) {
    throw std::runtime_error(
        "[Client] An error occurred while initializing ENet.");
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
    throw std::runtime_error("[Client] An error occurred while trying to "
                             "create an ENet _client host.");
  }

  ENetAddress address;
  enet_address_set_host(&address, host.c_str());
  address.port = port;

  _peer = enet_host_connect(_client, &address, 2, 0);
  if (_peer == nullptr) {
    throw std::runtime_error(
        "[Client] No available _peers for initiating an ENet connection.");
  }

  ENetEvent event;
  if (enet_host_service(_client, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    std::cout << "[Client] Connection to " << host << ":" << port
              << " succeeded." << std::endl;
  } else {
    enet_peer_reset(_peer);
    throw std::runtime_error("[Client] Connection to " + host + ":" +
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
      std::cout << "[Client] Disconnection succeeded." << std::endl;
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
      std::cout << "[Client] Disconnected from server." << std::endl;
      return;
    default:
      break;
    }
  }
}

void Client::sendMessage(ENetPacket *packet) {
  if (_peer != nullptr) {
    if (enet_peer_send(_peer, 0, packet) < 0)
      enet_packet_destroy(packet);
  }
}

int Client::_handleReceive(ENetEvent &event, Engine &eng) {
  if (event.packet->dataLength < sizeof(MessageHeader))
    return -1;
  MessageHeader *msgHeader = (MessageHeader *)(event.packet->data);
  size_t bodyLen = event.packet->dataLength - sizeof(MessageHeader);
  void *msgBody = (void *)(event.packet->data + sizeof(MessageHeader));
  switch (msgHeader->type) {
  case PLR_CO: {
    if (bodyLen < sizeof(MessagePlayerCo))
      return -1;
    eng.receiveMsg(static_cast<MessagePlayerCo *>(msgBody));
    break;
  }
  case PLR_DISCO: {
    if (bodyLen < sizeof(MessagePlayerDisco))
      return -1;
    eng.receiveMsg(static_cast<MessagePlayerDisco *>(msgBody));
    break;
  }
  case PLR_UPDATE: {
    if (bodyLen < sizeof(MessagePlayerUpdate))
      return -1;
    eng.receiveMsg(static_cast<MessagePlayerUpdate *>(msgBody));
    break;
  }
  case PLR_ID: {
    if (bodyLen < sizeof(MessagePlayerID))
      return -1;
    eng.receiveMsg(static_cast<MessagePlayerID *>(msgBody));
    break;
  }
  case MOB_UPDATE: {
    if (bodyLen < sizeof(MessageMobUpdate))
      return -1;
    eng.receiveMsg(static_cast<MessageMobUpdate *>(msgBody));
    break;
  }
  case MOB_HIT: {
    if (bodyLen < sizeof(MessageMobHit))
      return -1;
    eng.receiveMsg(static_cast<MessageMobHit *>(msgBody));
    break;
  }
  case MOB_ATTACK: {
    if (bodyLen < sizeof(MessageMobAttack))
      return -1;
    eng.receiveMsg(static_cast<MessageMobAttack *>(msgBody));
    break;
  }
  case GAME_STATE: {
    if (bodyLen < sizeof(MessageGameState))
      return -1;
    eng.receiveMsg(static_cast<MessageGameState *>(msgBody));
    break;
  }
  case MAP: {
    if (bodyLen < sizeof(MessageMap))
      return -1;
    eng.receiveMsg(static_cast<MessageMap *>(msgBody));
    break;
  }
  default:
    return -1;
  }
  return 0;
}
