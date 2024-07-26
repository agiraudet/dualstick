#ifndef SERVER_HPP
#define SERVER_HPP

#include <enet/enet.h>
#include <unordered_map>

#include "User.hpp"

class Server {
public:
  Server();
  ~Server();

  void init();
  void deinit();
  void run();

private:
  void _handleConnect(ENetEvent &event);
  void _handleReceive(ENetEvent &event);
  void _handleDisconnect(ENetEvent &event);
  void _distributeToOther(int authorId, ENetPacket *packet);
  void _distributeToAll(ENetPacket *packet);

  ENetAddress _address;
  ENetHost *_server;
  std::unordered_map<ENetPeer *, User> _users;
};

#endif
