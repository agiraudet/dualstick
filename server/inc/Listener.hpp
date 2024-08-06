#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <cstdint>
#include <deque>
#include <enet/enet.h>
#include <unordered_map>

enum msgDest { SINGLE, ALL, EXCEPT };

struct t_msg {
  ENetPeer *peer;
  ENetPacket *packet;
  msgDest dest;
};

class Server;

class Listener {
public:
  Listener();
  ~Listener();

  bool connect(void);
  void disconnect(void);
  bool connected(void) const;
  ENetHost *getHost(void);
  void setAddress(uint32_t adress, uint16_t port);
  void msgOutAdd(int id, ENetPacket *packet, msgDest dest);
  int msgOutProcess(void);
  void printAddress(ENetAddress const &address) const;
  void recv(Server &serv);
  int send(void);

private:
  void _disconnectAllPeer(void);
  void _msgDestroyAll(void);
  void _handleConnect(ENetEvent &event, Server &serv);
  void _handleDisconnect(ENetEvent &event, Server &serv);
  void _handleRecv(ENetEvent &event, Server &serv);

private:
  ENetHost *_host;
  ENetAddress _address;
  std::unordered_map<int, ENetPeer *> _peers;
  std::deque<t_msg> _msgOut;
};

#endif // !LISTENER_HPP
