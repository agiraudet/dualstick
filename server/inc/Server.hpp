#ifndef SERVER_HPP
#define SERVER_HPP

#include <enet/enet.h>
#include <unordered_map>
#include <vector>

#include "Message.hpp"
#include "User.hpp"

struct t_msg {
  ENetPeer *peer;
  ENetPacket *packet;
  bool ccAll;
  bool processed;
};

class Server {
public:
  Server();
  ~Server();

  void init();
  void deinit();
  void run();
  void stop();

private:
  void _msgOutProcess(void);
  void _msgOutClean(void);
  void _msgOutAdd(ENetPeer *perr, ENetPacket *packet, bool ccAll);

  void _disconnectAllClients(void);
  void _handleConnect(ENetEvent &event);
  void _handleReceive(ENetEvent &event);
  void _handleDisconnect(ENetEvent &event);
  void _distributeToOther(ENetPeer *author, ENetPacket *packet);
  void _distributeToAll(ENetPacket *packet);
  void _sendGameSateToAll(void);
  MessageGameState _craftMsgGameState(void);

  bool _running;
  ENetAddress _address;
  ENetHost *_server;
  std::unordered_map<ENetPeer *, User> _users;
  std::vector<t_msg> _msgOut;
};

extern Server *g_serverInstance;
void signalHandler(int signum);

#endif
