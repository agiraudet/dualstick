#include "Map.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Map::Map() : _width(0), _height(0) {}

Map::~Map() {}

bool Map::loadFromFile(const std::string &filename) {
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
  return true;
}

int Map::getTile(int x, int y) const {
  if (x < 0 || x >= _width || y < 0 || y >= _height) {
    throw std::out_of_range("Position out of bounds");
  }
  return _mapData[y][x];
}

int Map::getWidth() const { return _width; }

int Map::getHeight() const { return _height; }

bool Map::isCollidable(int x, int y) const {
  return _checkCollision(getTile(x, y));
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

bool Map::_checkCollision(int tileIndex) const { return tileIndex >= 3; }
