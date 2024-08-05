#ifndef SERVER_HPP
#define SERVER_HPP

#include <enet/enet.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "Map.hpp"
#include "Message.hpp"
#include "MobFactory.hpp"
#include "User.hpp"

enum msgDest { SINGLE, ALL, EXCEPT };

struct t_msg {
  ENetPeer *peer;
  ENetPacket *packet;
  msgDest dest;
};

class Server {
public:
  Server();
  ~Server();

  void init();
  void deinit();
  void run();
  void stop();
  int loadGame(std::string const &mapFile);

private:
  void _updateGameState(void);
  void _msgOutProcess(void);
  void _msgOutAdd(ENetPeer *peer, ENetPacket *packet, msgDest dest);
  void _loadMap(std::string const &path);
  void _disconnectAllClients(void);
  void _handleConnect(ENetEvent &event);
  void _handleReceive(ENetEvent &event);
  void _handleDisconnect(ENetEvent &event);
  void _distributeToOther(ENetPeer *author, ENetPacket *packet);
  void _distributeToAll(ENetPacket *packet);
  void _sendGameSateToAll(void);
  void _sendMap(ENetPeer *peer);
  void _craftMsgGameState(void);

  bool _running;
  ENetAddress _address;
  ENetHost *_server;
  Map _map;
  std::unordered_map<ENetPeer *, User> _users;
  std::vector<t_msg> _msgOut;
  MobFactory _hive;
  MessageGameState _msgGameState;
};

extern Server *g_serverInstance;
void signalHandler(int signum);

#endif
