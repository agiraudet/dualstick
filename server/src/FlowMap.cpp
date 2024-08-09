#include "FlowMap.hpp"
#include <chrono>
#include <climits>
#include <iomanip>
#include <mutex>

FlowMap::FlowMap(Map const &map)
    : _map(map), _playerVec(nullptr), _running(false) {}

FlowMap::~FlowMap(void) {}

void FlowMap::init(void) {
  int mapSize = _map.getSize();
  _dataA.resize(mapSize);
  for (int i = 0; i < mapSize; ++i) {
    _dataA[i].resize(_map.getRowSize(i), INT_MAX);
  }
  _width = _dataA[0].size();
  _height = _dataA.size();
  _dataB = _dataA;
  _writeBuffer.store(&_dataA);
  _readBuffer.store(&_dataB);
}

void FlowMap::reset(void) {
  for (auto &row : (*_writeBuffer)) {
    std::fill(row.begin(), row.end(), INT_MAX);
  }
}

int FlowMap::getValue(int x, int y) { return (*_readBuffer)[y][x]; }

Vector FlowMap::getDir(int x, int y) {
  if (x < 0 || x >= _width || y < 0 || y >= _height)
    return Vector(0, 0);
  std::vector<Vector> directions = {
      Vector(0, -1), // N
      Vector(1, 0),  // E
      Vector(0, 1),  // S
      Vector(-1, 0), // W
  };
  int minValue = (*_readBuffer)[y][x];
  Vector direction(0, 0);
  for (const auto &dir : directions) {
    int newX = x + dir.x;
    int newY = y + dir.y;
    if (newX >= 0 && newX < _width && newY >= 0 && newY < _height) {
      if ((*_readBuffer)[newY][newX] < minValue) {
        minValue = (*_readBuffer)[newY][newX];
        direction = dir;
      }
    }
  }
  return direction;
}

void FlowMap::print(void) {
  for (const auto &row : (*_readBuffer)) {
    for (const auto &elem : row) {
      if (elem > 999)
        std::cout << "   ";
      else if (elem > 99)
        std::cout << "xx ";
      else
        std::cout << std::setw(2) << elem << " ";
    }
    std::cout << std::endl;
  }
}

void FlowMap::refresh(int x, int y) { _update(0, x, y); }

void FlowMap::_update(int value, int x, int y) {
  if (value >= (*_writeBuffer)[y][x] || x < 0 || x >= _width || y < 0 ||
      y >= _height)
    return;
  if (_map.checkCollision(_map.getTile(x, y)))
    return;
  (*_writeBuffer)[y][x] = value;
  _update(value + 2, x - 1, y - 1);
  _update(value + 1, x, y - 1);
  _update(value + 2, x + 1, y - 1);
  _update(value + 1, x - 1, y);
  _update(value + 1, x + 1, y);
  _update(value + 2, x - 1, y + 1);
  _update(value + 1, x, y + 1);
  _update(value + 2, x + 1, y + 1);
}

void FlowMap::updatePlayerVec(std::vector<Player *> *playerVec) {
  std::lock_guard<std::mutex> guard(_playerMutex);
  _playerVec = playerVec;
}

void FlowMap::startUpdating(void) {
  if (_running)
    return;
  _running = true;
  _updateThread = std::thread(&FlowMap::_updateLoop, this);
}

void FlowMap::stopUpdating(void) {
  if (!_running)
    return;
  _running = false;
  if (_updateThread.joinable()) {
    _updateThread.join();
  }
}

void FlowMap::_updateLoop() {
  while (_running) {
    {
      {
        std::lock_guard<std::mutex> plrGuard(_playerMutex);
        if (_playerVec) {
          reset();
          for (auto p : *_playerVec) {
            Vector const &pos = p->getPos();
            refresh(pos.x / _map.getTileSize(), pos.y / _map.getTileSize());
          }
        }
      }
      _swapBuffers();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void FlowMap::_swapBuffers(void) {
  auto tmp = _writeBuffer.load();
  _writeBuffer.store(_readBuffer);
  _readBuffer.store(tmp);
}
