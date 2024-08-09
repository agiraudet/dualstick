#include "Map.hpp"
#include "Message.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

Map::Map() : _width(0), _height(0), _tileSize(32), _loaded(false) {}

Map::~Map() {}

bool Map::loadFromFile(const std::string &filename) {
  printf("Loading map from file: %s\n", filename.c_str());
  std::ifstream file(filename);
  if (!file.is_open()) {
    return false;
  }
  std::string line;
  _data.clear();
  _height = 0;
  while (std::getline(file, line)) {
    std::vector<int> row = _parseLine(line);
    if (_height == 0) {
      _width = row.size();
    } else if (row.size() != _width) {
      file.close();
      throw std::runtime_error("Inconsistent row widths in map file");
    }
    _data.push_back(row);
    _height++;
  }
  file.close();
  _loaded = true;
  return true;
}

bool Map::loadFromMsg(void) {
  printf("Loading map from msg\n");
  int nMsg = _msgMap.size();
  if (nMsg <= 0 || nMsg != _msgMap[0].nPack) {
    std::cerr << "Expected " << _msgMap[0].nPack << " map package but got "
              << nMsg << std::endl;
    return false;
  }
  _data.clear();
  _width = _msgMap[0].width;
  _tileSize = _msgMap[0].tileSize;
  _height = 0;
  std::vector<int> row;
  for (int n = 0; n < nMsg; n++) {
    auto it =
        std::find_if(_msgMap.begin(), _msgMap.end(),
                     [n](const MessageMap &msg) { return msg.idPack == n; });
    if (it == _msgMap.end()) {
      _data.clear();
      std::cerr << "Missing map packets !" << std::endl;
      return false;
    }
    const MessageMap &msg = *it;
    int dataIndex = 0;
    while (dataIndex < msg.dataLen) {
      row.push_back(msg.data[dataIndex++]);
      if (row.size() == _width) {
        _data.push_back(row);
        row.clear();
        _height++;
      }
    }
  }
  if (!row.empty()) {
    _data.push_back(row);
    _height++;
  }
  _loaded = true;
  return true;
}

bool Map::isLoaded(void) { return _loaded; }

bool Map::craftMsg(void) {
  int nPack =
      std::ceil(static_cast<double>(_data.size() * _width) / MSGMAP_DATALEN);
  int row = 0;
  int r = 0;
  _msgMap.clear();
  for (int i = 0; i < nPack; i++) {
    MessageMap msg = {i, nPack, _width, _tileSize, 0, {0}};
    for (int d = 0; d < MSGMAP_DATALEN; d++) {
      msg.data[d] = _data[row][r++];
      msg.dataLen++;
      if (r >= _width) {
        row++;
        r = 0;
        if (row >= _data.size()) {
          break;
        }
      }
    }
    _msgMap.push_back(msg);
  }
  printf("Map: crafted %lu msg\n", _msgMap.size());
  return true;
}

void Map::rcvMsg(MessageMap &msg) {
  printf("Rcv map packet %d/%d\n", msg.idPack + 1, msg.nPack);
  _msgMap.push_back(msg);
}

int Map::countMissingMsg(void) {
  int nMsg = _msgMap.size();
  if (nMsg == 0)
    return -1;
  return _msgMap[0].nPack - nMsg;
}

void Map::setTileSize(int size) { _tileSize = size; }

std::vector<MessageMap> const &Map::getMsg(void) { return _msgMap; }

int Map::getSize(void) const { return _data.size(); }

int Map::getRowSize(int row) const { return _data[row].size(); }

int Map::getTile(int x, int y) const {
  if (x < 0 || x >= _width || y < 0 || y >= _height) {
    return 0;
  }
  return _data[y][x];
}

int Map::getWidth() const { return _width; }

int Map::getHeight() const { return _height; }

int Map::getTileSize() const { return _tileSize; }

bool Map::pointIsColliding(int x, int y) const {
  return checkCollision(getTile(x / _tileSize, y / _tileSize));
}

bool Map::boxIsColliding(int x, int y, int w, int h) const {
  int tileTL = getTile(x / _tileSize, y / _tileSize);
  int tileTR = getTile((x + w) / _tileSize, y / _tileSize);
  int tileBL = getTile(x / _tileSize, (y + h) / _tileSize);
  int tileBR = getTile((x + w) / _tileSize, (y + h) / _tileSize);
  return (checkCollision(tileTL) || checkCollision(tileTR) ||
          checkCollision(tileBL) || checkCollision(tileBR));
}

bool Map::isCollidable(int x, int y) const {
  return checkCollision(getTile(x, y));
}

std::vector<int> Map::_parseLine(const std::string &line) const {
  std::vector<int> row;
  std::istringstream stream(line);
  int tile;
  while (stream >> tile) {
    row.push_back(tile);
  }
  return row;
}

bool Map::checkCollision(int tileIndex) const {
  if (tileIndex >= 46 && tileIndex <= 49)
    return false;
  else if (tileIndex >= 56)
    return false;
  return true;
}
int Map::rayCast(Player &shooter,
                 std::unordered_map<int, std::shared_ptr<Mob>> &shooty,
                 int &hitX, int &hitY) {
  if (shooter.weapon->fire()) {
    const double rayInc = 1;
    const double maxDist = shooter.weapon->range;
    const double angle = shooter.getAngle();
    Vector ray;
    for (double t = 0; t < maxDist; t += rayInc) {
      ray.x = shooter.getPos().x + t * cos(angle);
      ray.y = shooter.getPos().y + t * sin(angle);
      if (shooty.empty())
        return -1;
      for (auto &e : shooty) {
        if (ray.distance(e.second->getPos()) <= e.second->getSize()) {
          shooter.weapon->dealDamage(*e.second, t);
          hitX = ray.x;
          hitY = ray.y;
          return e.first;
        } else if (pointIsColliding(ray.x, ray.y)) {
          return -1;
        }
      }
    }
  }
  return -1;
}
