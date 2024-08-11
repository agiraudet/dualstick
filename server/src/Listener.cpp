#include "Listener.hpp"
#include "Message.hpp"
#include "Server.hpp"

#include <enet/enet.h>
#include <enet/types.h>
#include <iostream>
#include <ostream>

Listener::Listener(void) : _host(nullptr) {
  _address.host = ENET_HOST_ANY;
  _address.port = 45454;
}

Listener::~Listener(void) { disconnect(); }

bool Listener::connect(void) {
  if (_host)
    disconnect();
  _host = enet_host_create(&_address, 32, 2, 0, 0);
  if (!_host) {
    std::cerr << "[Listener] An error occurred while trying to create an ENet "
                 "server host."
              << std::endl;
    return false;
  }
  std::cout << "[Listener] Initialized: ";
  printAddress(_address);
  return true;
}

void Listener::disconnect(void) {
  _msgDestroyAll();
  if (_host != nullptr) {
    _disconnectAllPeer();
    enet_host_destroy(_host);
    _host = nullptr;
  }
}

bool Listener::connected(void) const { return (_host != nullptr); }

ENetHost *Listener::getHost(void) { return _host; }

void Listener::setAddress(uint32_t adress, uint16_t port) {
  _address.host = static_cast<enet_uint32>(adress);
  _address.port = static_cast<enet_uint16>(port);
}

void Listener::msgOutAdd(int id, ENetPacket *packet, msgDest dest) {
  ENetPeer *peer = nullptr;
  if (dest != ALL) {
    auto const &it = _peers.find(id);
    if (it != _peers.end())
      peer = it->second;
  }
  t_msg msg = {peer, packet, dest};
  _msgOut.push_back(msg);
}

int Listener::msgOutProcess(void) {
  if (_msgOut.empty())
    return 0;
  t_msg const &msg = _msgOut.front();
  switch (msg.dest) {
  case ALL:
    enet_host_broadcast(_host, 0, msg.packet);
    break;
  case EXCEPT:
    for (auto const &p : _peers) {
      if (p.second != msg.peer) {
        if (enet_peer_send(p.second, 0, msg.packet) < 0) {
          enet_packet_destroy(msg.packet);
          return -1;
        }
      }
    }
    break;
  case SINGLE:
    if (msg.peer && enet_peer_send(msg.peer, 0, msg.packet) < 0) {
      enet_packet_destroy(msg.packet);
      return -1;
    }
    break;
  default:
    return -1;
  }
  _msgOut.pop_front();
  return _msgOut.size();
}

int Listener::send(void) {
  int ret;
  do {
    ret = msgOutProcess();
  } while (ret > 0);
  return ret;
}

void Listener::_disconnectAllPeer(void) {
  for (auto &p : _peers) {
    enet_peer_disconnect(p.second, 0);
  }
  ENetEvent event;
  while (enet_host_service(_host, &event, 3000) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_DISCONNECT:
      std::cout << "[Listener] Peer disconnected: ";
      printAddress(event.peer->address);
      break;
    default:
      break;
    }
  }
}

void Listener::_msgDestroyAll(void) {
  for (t_msg &msg : _msgOut) {
    if (msg.packet)
      enet_packet_destroy(msg.packet);
  }
  _msgOut.clear();
}

void Listener::printAddress(ENetAddress const &address) const {
  std::cout << (address.host & 0xFF) << "." << ((address.host >> 8) & 0xFF)
            << "." << ((address.host >> 16) & 0xFF) << "."
            << ((address.host >> 24) & 0xFF) << ":" << address.port
            << std::endl;
}

void Listener::recv(Server &serv) {
  ENetEvent event;
  while (enet_host_service(_host, &event, 0) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      _handleConnect(event, serv);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      _handleRecv(event, serv);
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      _handleDisconnect(event, serv);
      break;
    default:
      std::cout << "WEIRD " << event.type << std::endl;
      break;
    }
  }
}

void Listener::_handleConnect(ENetEvent &event, Server &serv) {
  static int peerId = 0;
  int newPeerId = peerId++;
  _peers[newPeerId] = event.peer;
  std::cout << "[Listener] New connection: ";
  printAddress(event.peer->address);
  std::cout << "[Listener] New player got assigned id: " << newPeerId
            << std::endl;
  MessagePlayerID msgId = {newPeerId};
  ENetPacket *packetId = packageMessage(msgId, PLR_ID, true);
  msgOutAdd(newPeerId, packetId, SINGLE);
  serv.playerAdd(newPeerId);
}

void Listener::_handleDisconnect(ENetEvent &event, Server &serv) {
  int oldPeerId = -1;
  for (auto const &p : _peers) {
    if (p.second == event.peer)
      oldPeerId = p.first;
  }
  std::cout << "[Listener] Player " << oldPeerId << " disconnected"
            << std::endl;
  if (oldPeerId >= 0) {
    serv.playerRemove(oldPeerId);
    _peers.erase(oldPeerId);
  }
  MessagePlayerDisco msgDisco = {oldPeerId};
  ENetPacket *packetDisco = packageMessage(msgDisco, PLR_DISCO, true);
  msgOutAdd(oldPeerId, packetDisco, ALL);
}

void Listener::_handleRecv(ENetEvent &event, Server &serv) {
  if (event.packet->dataLength < sizeof(MessageHeader)) {
    std::cerr << "Received packet with invalid length" << std::endl;
    enet_packet_destroy(event.packet);
    return;
  }
  MessageHeader *msgHeader = (MessageHeader *)event.packet->data;
  switch (msgHeader->type) {
  case PLR_UPDATE: {
    if (event.packet->dataLength !=
        sizeof(MessageHeader) + sizeof(MessageEntityUpdate)) {
      std::cerr << "Received PLR_UPDATE packet with invalid length"
                << std::endl;
      break;
    }
    MessageEntityUpdate *msgUpdate =
        (MessageEntityUpdate *)(event.packet->data + sizeof(MessageHeader));
    serv.playerUpdate(msgUpdate->id, msgUpdate->pos, msgUpdate->vel,
                      msgUpdate->angle);
    break;
  }
  case PLR_SHOOT: {
    if (event.packet->dataLength !=
        sizeof(MessageHeader) + sizeof(MessagePlayerShoot)) {
      std::cerr << "Received PLR_SHOOT packet with invalid length" << std::endl;
      break;
    }
    MessagePlayerShoot *msgShoot =
        (MessagePlayerShoot *)(event.packet->data + sizeof(MessageHeader));
    serv.playerUpdate(msgShoot->id, msgShoot->pos, msgShoot->vel,
                      msgShoot->angle);
    serv.playerShoot(msgShoot->id);
    break;
  }
  default:
    break;
  }
  enet_packet_destroy(event.packet);
}
