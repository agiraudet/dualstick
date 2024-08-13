#ifndef MAP_H
#define MAP_H

#include <memory>
#include <string>
#include <vector>

#include "Entity.hpp"
#include "Message.hpp"
#include "Shop.hpp"

class Player;
class Mob;

class Map {
public:
  Map();
  ~Map();

  bool loadFromFile(const std::string &filename);
  bool loadFromMsg(void);
  bool isLoaded(void);
  bool loadShopsWIP(void);

  bool craftMsg(void);
  bool craftMsgShop(void);
  void rcvMsg(MessageMap &msg);
  void rcvMsg(MessageMapShop &msg);
  int countMissingMsg(void);
  std::vector<MessageMap> const &getMsg(void);
  std::vector<MessageMapShop> const &getMsgShop(void);
  std::vector<std::shared_ptr<IShop>> const &getShops(void) const;

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
  bool lineOfSight(Entity &entA, Entity &entB, double maxDist = 0.f);
  bool lineOfSight(Vector pos, double angle, Entity &entB,
                   double maxDist = 0.f);
  std::shared_ptr<Entity> rayCast(Entity &shooter, EntityManager<Mob> &shooties,
                                  double maxDist, int &hitX, int &hitY);
  bool playerInterract(Player &player);

protected:
  std::vector<int> _parseLine(const std::string &line) const;

protected:
  std::vector<std::shared_ptr<IShop>> _shops;
  std::vector<std::vector<int>> _data;
  std::vector<MessageMap> _msgMap;
  std::vector<MessageMapShop> _msgMapShop;
  int _width;
  int _height;
  int _tileSize;
  bool _loaded;
};

#endif // MAP_H
