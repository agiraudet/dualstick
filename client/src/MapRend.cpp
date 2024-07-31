#include "MapRend.hpp"
#include <SDL2/SDL_rect.h>
#include <cstdio>

MapRend::MapRend(Atlas *atlas, const std::string &tileSet)
    : _atlas(atlas), _tileSet(tileSet) {
  const Texture &tileTexture = _atlas->getTexture(tileSet);
  setTileSize(tileTexture.frame.w, tileTexture.frame.h);
}

void MapRend::setTileSize(int tileWidth, int tileHeight) {
  _tileWidth = tileWidth;
  _tileHeight = tileHeight;
  _pixelWidth = tileWidth * _width;
  _pixelHeight = tileHeight * _height;
}

void MapRend::createTiles(void) {
  std::vector<Tile> line;
  for (int y = 0; y < _height; y++) {
    line.clear();
    for (int x = 0; x < _width; x++) {
      Tile tile = {{_tileWidth * x, _tileHeight * y, _tileWidth, _tileHeight},
                   getTile(x, y)};
      line.push_back(tile);
    }
    _tileData.push_back(line);
  }
  printf("loaded %ld tiles\n", _tileData.size() * _tileData[0].size());
}

void MapRend::render(SDL_Rect &camera) {
  for (const auto &tileLine : _tileData) {
    for (const auto &tile : tileLine) {
      if (SDL_HasIntersection(&camera, &tile.rect) == SDL_TRUE) {
        _atlas->drawTextureF(_tileSet, tile.frame, tile.rect.x - camera.x,
                             tile.rect.y - camera.y);
      }
    }
  }
}
