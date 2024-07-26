#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <enet/enet.h>
#include <string>

class Client {
public:
  Client();
  ~Client();

  void init(const std::string &host, enet_uint16 port);
  void deinit();
  void getEvent();
  void sendMessage(const std::string &message);

private:
  void _handleReceive(ENetEvent &event);
  ENetHost *_client;
  ENetPeer *_peer;
};
#endif
