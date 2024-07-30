#ifndef USER_HPP
#define USER_HPP

#include <enet/enet.h>

#include "Player.hpp"
#include "Vector.hpp"

class User {
  static int count;

public:
  User(void);
  User(ENetPeer *peer);
  int getId(void) const;
  void send(ENetPacket *packet) const;
  ENetPeer *getPeer() const;
  void updatePlayer(Vector &pos, Vector &vel);

private:
  int _id;
  ENetPeer *_peer;

public:
  Player player;
};

#endif
