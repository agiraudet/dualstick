#ifndef MAPREND_HPP
#define MAPREND_HPP

#include "Atlas.hpp"
#include "Map.hpp"
#include <vector>

struct Tile {
  SDL_Rect rect;
  int frame;
};

class MapRend : public Map {

public:
  MapRend(Atlas *atlas, const std::string &tileSet);
  void createTiles(void);
  void render(SDL_Rect &camera);

  /*public:*/
  /*  void debug_render(SDL_Rect &camera);*/

private:
  Atlas *_atlas;
  const std::string _tileSet;
  int _pixelWidth;
  int _pixelHeight;
  std::vector<std::vector<Tile>> _tileData;
};
#endif // !MAPREND_HPP
