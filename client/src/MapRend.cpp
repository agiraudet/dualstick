#include "MapRend.hpp"
#include <SDL2/SDL_rect.h>
#include <cstdio>

MapRend::MapRend(void) {
  _pixelWidth = _tileSize * _width;
  _pixelHeight = _tileSize * _height;
}

void MapRend::createTiles(void) {
  std::vector<Tile> line;
  for (int y = 0; y < _height; y++) {
    line.clear();
    for (int x = 0; x < _width; x++) {
      Tile tile = {{_tileSize * x, _tileSize * y, _tileSize, _tileSize},
                   getTile(x, y)};
      line.push_back(tile);
    }
    _tileData.push_back(line);
  }
  printf("loaded %ld tiles\n", _tileData.size() * _tileData[0].size());
}

void MapRend::render(SDL_Rect &camera, Tex &tex) {
  for (const auto &tileLine : _tileData) {
    for (const auto &tile : tileLine) {
      if (SDL_HasIntersection(&camera, &tile.rect) == SDL_TRUE) {
        tex.draw(tile.rect.x - camera.x, tile.rect.y - camera.y, tile.frame);
      }
    }
  }
}
