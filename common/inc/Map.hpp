#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

class Map {
public:
  Map();
  ~Map();

  bool loadFromFile(const std::string &filename);
  int getTile(int x, int y) const;
  int getWidth() const;
  int getHeight() const;
  bool isCollidable(int x, int y) const;

protected:
  std::vector<int> _parseLine(const std::string &line) const;
  bool _checkCollision(int tileIndex) const;

protected:
  std::vector<std::vector<int>> _mapData;
  int _width;
  int _height;
};

#endif // MAP_H
