#include <cstdio>
#include <cstdlib>
#include <enet/enet.h>

#include "User.hpp"

int User::count = 0;

User::User(void) : _peer(nullptr) { _id = count++; }

User::User(ENetPeer *peer) : _peer(peer) { _id = count; }

int User::getId(void) const { return _id; }

void User::send(ENetPacket *packet) const {
  int ret = enet_peer_send(_peer, 0, packet);
  if (ret < 0)
    enet_packet_destroy(packet);
}

ENetPeer *User::getPeer() const { return _peer; }

void User::updatePlayer(float angle, Vector &pos, Vector &vel) {
  player.setAngle(angle);
  player.setPos(pos);
  player.setVel(vel);
}
