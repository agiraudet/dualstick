#include "Atlas.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include <ostream>

Atlas::Atlas(SDL_Renderer *renderer) : _renderer(renderer) {}

Atlas::~Atlas() { cleanup(); }

const Texture &Atlas::getTexture(const std::string &id) {
  return _textures[id];
}

bool Atlas::loadTexture(const std::string &id, const std::string &path) {
  SDL_Texture *texture = IMG_LoadTexture(_renderer, path.c_str());
  if (!texture) {
    std::cerr << " IMG_LoadTexture error: " << SDL_GetError() << std::endl;
    return false;
  }
  int w, h;
  SDL_QueryTexture(texture, NULL, NULL, &w, &h);
  _textures[id] = {texture, 0, {0, 0, w, h}, w, h};
  std::cout << "Loaded texture " << id << std::endl;
  return true;
}

bool Atlas::loadTexture(const std::string &id, const std::string &path,
                        size_t vFrameSize, size_t hFrameSize) {
  if (!loadTexture(id, path)) {
    return false;
  }
  Texture &tex = _textures[id];
  tex.nFrame = (tex.w / hFrameSize) * (tex.h / vFrameSize);
  tex.frame = {0, 0, static_cast<int>(hFrameSize),
               static_cast<int>(vFrameSize)};
  return true;
}

void Atlas::drawTexture(const std::string &id, int x, int y) {
  SDL_Rect dstRect = {x, y, _textures[id].frame.w, _textures[id].frame.h};
  SDL_RenderCopy(_renderer, _textures[id].texture, 0, &dstRect);
}

void Atlas::drawTexture(const std::string &id, int x, int y, float angle) {
  SDL_Rect dstRect = {x, y, _textures[id].frame.w, _textures[id].frame.h};
  SDL_Point center = {_textures[id].frame.w / 2, _textures[id].frame.h / 2};
  SDL_RenderCopyEx(_renderer, _textures[id].texture, 0, &dstRect, angle,
                   &center, SDL_FLIP_NONE);
}

/*void Atlas::drawTexture(const std::string &id, int x, int y, int scale) {*/
/*  SDL_Rect *srcRect = &_textures[id].frame;*/
/*  SDL_Rect dstRect = {x, y, srcRect->w * scale, srcRect->h * scale};*/
/*  SDL_RenderCopy(_renderer, _textures[id].texture, srcRect, &dstRect);*/
/*}*/

void Atlas::drawTexture(const std::string &id, int x, int y,
                        SDL_Rect &srcRect) {
  SDL_Rect dstRect = {x, y, srcRect.w, srcRect.h};
  SDL_RenderCopy(_renderer, _textures[id].texture, &srcRect, &dstRect);
}

void Atlas::drawTexture(const std::string &id, int x, int y, float angle,
                        SDL_Rect &srcRect) {
  SDL_Rect dstRect = {x, y, srcRect.w, srcRect.h};
  SDL_Point center = {dstRect.w / 2, dstRect.h / 2};
  SDL_RenderCopyEx(_renderer, _textures[id].texture, &srcRect, &dstRect, angle,
                   &center, SDL_FLIP_NONE);
}

/*void Atlas::drawTexture(const std::string &id, int x, int y, int scale,*/
/*                        SDL_Rect &srcRect) {*/
/*  SDL_Rect dstRect = {x, y, srcRect.w * scale, srcRect.h * scale};*/
/*  SDL_RenderCopy(_renderer, _textures[id].texture, &srcRect, &dstRect);*/
/*}*/

void Atlas::drawTextureF(const std::string &id, size_t nFrame, int x, int y) {
  Texture &tex = _textures[id];
  SDL_Rect srcRect = tex.frame;
  srcRect.x = (nFrame % (tex.w / tex.frame.w)) * tex.frame.w;
  srcRect.y = (nFrame / (tex.w / tex.frame.w)) * tex.frame.h;
  drawTexture(id, x, y, srcRect);
}

void Atlas::drawTextureF(const std::string &id, size_t nFrame, int x, int y,
                         float angle) {
  Texture &tex = _textures[id];
  SDL_Rect srcRect = tex.frame;
  srcRect.x = (nFrame % (tex.w / tex.frame.w)) * tex.frame.w;
  srcRect.y = (nFrame / (tex.w / tex.frame.w)) * tex.frame.h;
  drawTexture(id, x, y, angle, srcRect);
}

/*void Atlas::drawTexture(const std::string &id, size_t nFrame, int x, int y,*/
/*                        int scale) {*/
/*  Texture &tex = _textures[id];*/
/*  SDL_Rect srcRect = tex.frame;*/
/*  srcRect.x = (nFrame % (tex.w / tex.frame.w)) * tex.frame.w;*/
/*  srcRect.y = (nFrame / (tex.w / tex.frame.w)) * tex.frame.h;*/
/*  drawTexture(id, x, y, scale, srcRect);*/
/*}*/

void Atlas::drawTextureF(const std::string &id, size_t nFrame, int x, int y,
                         SDL_Rect &srcRect) {
  Texture &tex = _textures[id];
  srcRect.x = (nFrame % (tex.w / tex.frame.w)) * tex.frame.w;
  srcRect.y = (nFrame / (tex.w / tex.frame.w)) * tex.frame.h;
  drawTexture(id, x, y, srcRect);
}

void Atlas::drawTextureF(const std::string &id, size_t nFrame, int x, int y,
                         float angle, SDL_Rect &srcRect) {
  Texture &tex = _textures[id];
  srcRect.x = (nFrame % (tex.w / tex.frame.w)) * tex.frame.w;
  srcRect.y = (nFrame / (tex.w / tex.frame.w)) * tex.frame.h;
  SDL_Rect dstRect = {x, y, srcRect.w, srcRect.h};
  SDL_Point center = {dstRect.w / 2, dstRect.h / 2};
  SDL_RenderCopyEx(_renderer, _textures[id].texture, &srcRect, &dstRect, angle,
                   &center, SDL_FLIP_NONE);
}

/*void Atlas::drawTexture(const std::string &id, size_t nFrame, int x, int y,*/
/*                        int scale, SDL_Rect &srcRect) {*/
/*  Texture &tex = _textures[id];*/
/*  srcRect.x = (nFrame % (tex.w / tex.frame.w)) * tex.frame.w;*/
/*  srcRect.y = (nFrame / (tex.w / tex.frame.w)) * tex.frame.h;*/
/*  SDL_Rect dstRect = {x, y, srcRect.w * scale, srcRect.h * scale};*/
/*  SDL_Point center = {dstRect.w / 2, dstRect.h / 2};*/
/*  SDL_RenderCopyEx(_renderer, _textures[id].texture, &srcRect, &dstRect, 0,*/
/*                   &center, SDL_FLIP_NONE);*/
/*}*/

void Atlas::cleanup() {
  for (auto &pair : _textures) {
    SDL_DestroyTexture(pair.second.texture);
  }
  _textures.clear();
}
