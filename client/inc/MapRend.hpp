#ifndef MAPREND_HPP
#define MAPREND_HPP

#include "Map.hpp"
#include "Tex.hpp"
#include <vector>

struct Tile {
  SDL_Rect rect;
  int frame;
};

class MapRend : public Map {

public:
  MapRend(void);
  void createTiles(void);
  void render(SDL_Rect &camera, Tex &tex);

private:
  int _pixelWidth;
  int _pixelHeight;
  std::vector<std::vector<Tile>> _tileData;
};
#endif // !MAPREND_HPP
