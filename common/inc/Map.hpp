#ifndef MAP_H
#define MAP_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Message.hpp"

class Player;
class Mob;

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

  void setTileSize(int size);
  int getSize(void) const;
  int getRowSize(int row) const;
  int getTile(int x, int y) const;
  int getWidth() const;
  int getHeight() const;
  int getTileSize() const;
  bool isCollidable(int x, int y) const;
  bool pointIsColliding(int x, int y) const;
  bool boxIsColliding(int x, int y, int w, int h) const;
  bool checkCollision(int tileIndex) const;
  int rayCast(Player &shooter,
              std::unordered_map<int, std::shared_ptr<Mob>> &shooty, int &hitX,
              int &hitY);

protected:
  std::vector<int> _parseLine(const std::string &line) const;

protected:
  std::vector<std::vector<int>> _data;
  std::vector<MessageMap> _msgMap;
  int _width;
  int _height;
  int _tileSize;
  bool _loaded;
};

#endif // MAP_H
