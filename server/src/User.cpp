#include <cstdlib>
#include <enet/enet.h>

#include "User.hpp"

int User::count = 0;

User::User(void) : _peer(nullptr) { _id = count++; }

User::User(ENetPeer *peer) : _peer(peer) { _id = count++; }

int User::getId(void) const { return _id; }

void User::send(ENetPacket *packet) const { enet_peer_send(_peer, 0, packet); }

void User::send(const char *data, size_t dataLen) const {
  ENetPacket *packet =
      enet_packet_create(data, dataLen, ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(_peer, 0, packet);
  enet_packet_destroy(packet);
}

ENetPeer *User::getPeer() const { return _peer; }
