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
    if (enet_peer_send(_peer, 0, packet) < 0)
      enet_packet_destroy(packet);
  }
}

void Client::_handleReceive(ENetEvent &event, Engine &eng) {
  MessageHeader *msgHeader = (MessageHeader *)(event.packet->data);
  void *msgBody = (void *)(event.packet->data + sizeof(MessageHeader));
  switch (msgHeader->type) {
  case PLR_CO: {
    MessagePlayerCo *msgCo = (MessagePlayerCo *)msgBody;
    eng.addPlayer(msgCo->id);
    break;
  }
  case PLR_DISCO: {
    MessagePlayerDisco *msgDisco = (MessagePlayerDisco *)msgBody;
    eng.removePlayer(msgDisco->id);
    break;
  }
  case PLR_UPDATE: {
    MessagePlayerUpdate *msgUpdate = (MessagePlayerUpdate *)msgBody;
    eng.updatePlayer(msgUpdate);
    break;
  }
  case PLR_ID: {
    MessagePlayerID *msgID = (MessagePlayerID *)msgBody;
    eng.setPlayerId(msgID->id);
    printf("received ID: %d\n", msgID->id);
    break;
  }
  case GAME_STATE: {
    MessageGameState *msgSate = (MessageGameState *)msgBody;
    for (int i = 0; i < msgSate->nplayer; i++) {
      MessagePlayerUpdate *plr = &msgSate->players[i];
      eng.updatePlayer(plr);
    }
    break;
  }
  case MAP: {
    MessageMap msgMap =
        deserializeMessage<MessageMap>((const char *)event.packet->data);
    eng.receivedMap(msgMap);
    break;
  }
  default:
    break;
  }
}
