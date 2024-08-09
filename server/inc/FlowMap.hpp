#ifndef FLOWMAP_HPP
#define FLOWMAP_HPP

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include "Map.hpp"
#include "Player.hpp"

class FlowMap {
public:
  FlowMap(Map const &map);
  ~FlowMap(void);

public:
  void refresh(int x, int y);
  void reset(void);
  void init(void);
  int getValue(int x, int y);
  void print(void);
  Vector getDir(int x, int y);
  void startUpdating(void);
  void stopUpdating(void);
  void updatePlayerVec(std::vector<Player *> *playerVec);

private:
  void _update(int value, int x, int y);
  void _updateLoop(void);
  void _swapBuffers(void);

private:
  Map const &_map;
  std::vector<std::vector<int>> _dataA;
  std::vector<std::vector<int>> _dataB;
  std::vector<Player *> *_playerVec;
  std::atomic<std::vector<std::vector<int>> *> _readBuffer;
  std::atomic<std::vector<std::vector<int>> *> _writeBuffer;
  int _width;
  int _height;

private:
  std::mutex _playerMutex;
  std::thread _updateThread;
  std::atomic<bool> _running;
};

#endif // !FLOWMAP_HPP
