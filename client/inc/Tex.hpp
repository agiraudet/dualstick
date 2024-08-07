#ifndef TEX_HPP
#define TEX_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <string>

class Tex {
public:
  Tex(void);
  Tex(SDL_Renderer *renderer, const std::string &src);
  Tex(SDL_Renderer *renderer, const std::string &src, int frameSize);
  Tex(Tex const &other);
  ~Tex(void);
  Tex &operator=(const Tex &other);

  void setFrameSize(int frameSize);
  void setSrc(std::string const &src);
  void setRenderer(SDL_Renderer *renderer);
  int getFrameSize(void) const;
  bool load(const std::string &src);
  void unload(void);
  void selectFrame(int frame);
  void draw(int x, int y);
  void draw(int x, int y, int frame);
  void drawRot(int x, int y, float angle);
  void drawRot(int x, int y, int frame, float angle);

private:
  std::string _src;
  SDL_Renderer *_renderer;
  SDL_Texture *_tex;
  SDL_Rect _frame;
  int _currentFrame;
  int _nFrame;
  int _width;
  int _height;
};

#endif
