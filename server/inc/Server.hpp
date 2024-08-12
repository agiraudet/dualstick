#ifndef SERVER_HPP
#define SERVER_HPP

#include <enet/enet.h>
#include <string>

#include "Entity.hpp"
#include "FlowMap.hpp"
#include "Listener.hpp"
#include "Map.hpp"
#include "Message.hpp"

enum servStatus { LOADING, READY, RUNNING, STOPPED };

class Server {
public:
  Server();
  ~Server();

  void start();
  void stop();
  int loadGame(std::string const &mapFile);
  int playerAdd(void);
  int playerAdd(int id);
  void playerRemove(int id);
  void playerUpdate(int id, Vector &pos, Vector &vel, float angle);
  void playerShoot(int id);
  void playerReload(int id);

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
  servStatus _status;
  Listener _listener;
  Map _map;
  FlowMap _flow;
  MessageGameState _msgGameState;
  EntityManager<Player> _EMPlayer;
  EntityManager<Mob> _EMMob;
};

extern Server *g_serverInstance;
void signalHandler(int signum);

#endif
