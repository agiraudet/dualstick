#include "Map.hpp"
#include "Message.hpp"
#include <algorithm>
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
  _mapData.clear();
  _height = 0;
  while (std::getline(file, line)) {
    std::vector<int> row = _parseLine(line);
    if (_height == 0) {
      _width = row.size();
    } else if (row.size() != _width) {
      file.close();
      throw std::runtime_error("Inconsistent row widths in map file");
    }
    _mapData.push_back(row);
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
  _mapData.clear();
  _width = _msgMap[0].width;
  _tileSize = _msgMap[0].tileSize;
  _height = 0;
  std::vector<int> row;
  for (int n = 0; n < nMsg; n++) {
    auto it =
        std::find_if(_msgMap.begin(), _msgMap.end(),
                     [n](const MessageMap &msg) { return msg.idPack == n; });
    if (it == _msgMap.end()) {
      _mapData.clear();
      std::cerr << "Missing map packets !" << std::endl;
      return false;
    }
    const MessageMap &msg = *it;
    int dataIndex = 0;
    while (dataIndex < msg.dataLen) {
      row.push_back(msg.data[dataIndex++]);
      if (row.size() == _width) {
        _mapData.push_back(row);
        row.clear();
        _height++;
      }
    }
  }
  if (!row.empty()) {
    _mapData.push_back(row);
    _height++;
  }
  _loaded = true;
  return true;
}

bool Map::isLoaded(void) { return _loaded; }

bool Map::craftMsg(void) {
  int nPack =
      std::ceil(static_cast<double>(_mapData.size() * _width) / MSGMAP_DATALEN);
  int row = 0;
  int r = 0;
  _msgMap.clear();
  for (int i = 0; i < nPack; i++) {
    MessageMap msg = {i, nPack, _width, _tileSize, 0, {0}};
    for (int d = 0; d < MSGMAP_DATALEN; d++) {
      msg.data[d] = _mapData[row][r++];
      msg.dataLen++;
      if (r >= _width) {
        row++;
        r = 0;
        if (row >= _mapData.size()) {
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

int Map::getTile(int x, int y) {
  if (x < 0 || x >= _width || y < 0 || y >= _height) {
    /*throw std::out_of_range("Position out of bounds");*/
    return 0;
  }
  /*_debugCoord.emplace_back(x, y);*/
  return _mapData[y][x];
}

int Map::getWidth() const { return _width; }

int Map::getHeight() const { return _height; }

int Map::getTileSize() const { return _tileSize; }

bool Map::boxIsColliding(int x, int y, int w, int h) {
  /*_debugCoord.clear();*/
  int tileTL = getTile(x / _tileSize, y / _tileSize);
  int tileTR = getTile((x + w) / _tileSize, y / _tileSize);
  int tileBL = getTile(x / _tileSize, (y + h) / _tileSize);
  int tileBR = getTile((x + w) / _tileSize, (y + h) / _tileSize);
  /*printf("%d %d %d %d\n", tileTL, tileTR, tileBR, tileBL);*/
  return (_checkCollision(tileTL) || _checkCollision(tileTR) ||
          _checkCollision(tileBL) || _checkCollision(tileBR));
}

bool Map::isCollidable(int x, int y) { return _checkCollision(getTile(x, y)); }

std::vector<int> Map::_parseLine(const std::string &line) const {
  std::vector<int> row;
  std::istringstream stream(line);
  int tile;
  while (stream >> tile) {
    row.push_back(tile);
  }
  return row;
}

bool Map::_checkCollision(int tileIndex) const {
  if (tileIndex >= 46 && tileIndex <= 49)
    return false;
  else if (tileIndex >= 56)
    return false;
  return true;
}
