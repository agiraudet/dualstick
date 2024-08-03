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
  void setTileSize(int size);
  std::vector<MessageMap> const &getMsg(void);
  int getTile(int x, int y);
  int getWidth() const;
  int getHeight() const;
  int getTileSize() const;
  bool isCollidable(int x, int y);
  bool boxIsColliding(int x, int y, int w, int h);

protected:
  std::vector<int> _parseLine(const std::string &line) const;
  bool _checkCollision(int tileIndex) const;

public:
  void _updateFlow(int value, int x, int y);
  void _resetFlow(void);
  void _initFlow(void);
  int _getValueFlow(int x, int y);
  void _printFlow(void);
  Vector _getDirFlow(int x, int y);

protected:
  std::vector<std::vector<int>> _flowData;
  std::vector<std::vector<int>> _mapData;
  std::vector<MessageMap> _msgMap;
  int _width;
  int _height;
  int _tileSize;
  bool _loaded;

  /*protected:*/
  /*  std::vector<std::pair<int, int>> _debugCoord;*/
};

#endif // MAP_H
