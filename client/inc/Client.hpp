#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <enet/enet.h>
#include <string>

class Engine;

class Client {
public:
  Client();
  ~Client();

  void init(const std::string &host, enet_uint16 port);
  void deinit();
  void disconnect();
  void getEvent(Engine &eng);
  void sendMessage(const std::string &message);
  void sendMessage(ENetPacket *packet);

private:
  void _handleReceive(ENetEvent &event, Engine &eng);
  ENetHost *_client;
  ENetPeer *_peer;
};
#endif
