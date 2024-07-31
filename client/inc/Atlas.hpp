#ifndef ATLAS_HPP
#define ATLAS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <string>

struct Texture {
  SDL_Texture *texture;
  size_t nFrame;
  SDL_Rect frame;
  int w;
  int h;
};

class Atlas {
public:
  Atlas(SDL_Renderer *renderer);
  ~Atlas();

  const Texture &getTexture(const std::string &id);
  bool loadTexture(const std::string &id, const std::string &path);
  bool loadTexture(const std::string &id, const std::string &path,
                   size_t vFrameSize, size_t hFrameSize);
  void drawTexture(const std::string &id, int x, int y);
  void drawTexture(const std::string &id, int x, int y, float angle);
  /*void drawTexture(const std::string &id, int x, int y, int scale);*/
  void drawTexture(const std::string &id, int x, int y, SDL_Rect &srcRect);
  void drawTexture(const std::string &id, int x, int y, float angle,
                   SDL_Rect &srcRect);
  /*void drawTexture(const std::string &id, int x, int y, int scale,*/
  /*                 SDL_Rect &srcRect);*/
  void drawTextureF(const std::string &id, size_t nFrame, int x, int y);
  void drawTextureF(const std::string &id, size_t nFrame, int x, int y,
                    float angle);
  /*void drawTexture(const std::string &id, size_t nFrame, int x, int y,*/
  /*                 int scale);*/
  void drawTextureF(const std::string &id, size_t nFrame, int x, int y,
                    SDL_Rect &srcRect);
  void drawTextureF(const std::string &id, size_t nFrame, int x, int y,
                    float angle, SDL_Rect &srcRect);
  /*void drawTexture(const std::string &id, size_t nFrame, int x, int y,*/
  /*                 int scale, SDL_Rect &srcRect);*/
  void cleanup();

private:
  SDL_Renderer *_renderer;
  std::map<std::string, Texture> _textures;
};

#endif // ATLAS_HPP
