#ifndef SERVER_HPP
#define SERVER_HPP

#include "Listener.hpp"
#include "Map.hpp"
#include "Message.hpp"
#include "MobFactory.hpp"
#include <enet/enet.h>
#include <string>
#include <unordered_map>

class Server {
public:
  Server();
  ~Server();

  void start();
  void stop();
  int loadGame(std::string const &mapFile);
  void playerAdd(int id);
  void playerRemove(int id);
  void playerUpdate(int id, Vector &pos, Vector &vel, float angle);

private:
  void _run();
  void _updateGameState(void);
  void _loadMap(std::string const &path);
  void _handleConnect(ENetEvent &event);
  void _handleReceive(ENetEvent &event);
  void _handleDisconnect(ENetEvent &event);
  void _sendGameSateToAll(void);
  void _sendMap(int id);
  void _craftMsgGameState(void);

private:
  bool _running;
  Listener _listener;
  Map _map;
  std::unordered_map<int, Player> _players;
  MobFactory _hive;
  MessageGameState _msgGameState;
};

extern Server *g_serverInstance;
void signalHandler(int signum);

#endif
