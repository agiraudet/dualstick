#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

#include "Message.hpp"

class Map {
public:
  Map();
  ~Map();

  bool loadFromFile(const std::string &filename);
  bool loadFromMsg(void);
  bool isLoaded(void);
  bool craftMsg(void);
  void rcvMsg(MessageMap &msg);
  int countMissingMsg(void);
  std::vector<MessageMap> const &getMsg(void);
  int getTile(int x, int y) const;
  int getWidth() const;
  int getHeight() const;
  bool isCollidable(int x, int y) const;

protected:
  std::vector<int> _parseLine(const std::string &line) const;
  bool _checkCollision(int tileIndex) const;

protected:
  std::vector<std::vector<int>> _mapData;
  std::vector<MessageMap> _msgMap;
  int _width;
  int _height;
  bool _loaded;
};

#endif // MAP_H
