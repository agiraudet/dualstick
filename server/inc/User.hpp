#ifndef USER_HPP
#define USER_HPP

#include <enet/enet.h>

class User {
  static int count;

public:
  User(void);
  User(ENetPeer *peer);
  int getId(void) const;
  void send(ENetPacket *packet) const;
  void send(const char *data, size_t dataLen) const;
  ENetPeer *getPeer() const;

private:
  int _id;
  ENetPeer *_peer;
};

#endif
