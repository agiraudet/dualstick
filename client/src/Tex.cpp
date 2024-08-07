#include "Tex.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
Tex::Tex(void)
    : _src(""), _renderer(nullptr), _tex(nullptr), _frame({0, 0, 0, 0}),
      _currentFrame(0), _nFrame(0), _width(0), _height(0) {}

Tex::Tex(SDL_Renderer *renderer, const std::string &src)
    : _src(src), _renderer(renderer), _tex(nullptr), _frame({0, 0, 0, 0}),
      _currentFrame(0), _nFrame(0), _width(0), _height(0) {
  if (load(src))
    setFrameSize(_width);
}

Tex::Tex(SDL_Renderer *renderer, const std::string &src, int frameSize)
    : _src(src), _renderer(renderer), _tex(nullptr), _frame({0, 0, 0, 0}),
      _currentFrame(0), _nFrame(0), _width(0), _height(0) {
  if (load(src))
    setFrameSize(frameSize);
}

Tex::Tex(Tex const &other)
    : _src(other._src), _renderer(other._renderer), _tex(nullptr),
      _frame(other._frame), _currentFrame(other._currentFrame),
      _nFrame(other._nFrame), _width(other._width), _height(other._height) {
  load(_src);
}

Tex::~Tex(void) { unload(); }

Tex &Tex::operator=(const Tex &other) {
  if (this == &other) {
    return *this;
  }
  if (_tex)
    unload();
  _src = other._src;
  _renderer = other._renderer;
  _frame = other._frame;
  _currentFrame = other._currentFrame;
  _nFrame = other._nFrame;
  _width = other._width;
  _height = other._height;
  load(_src);
  return (*this);
}

void Tex::setSrc(std::string const &src) { _src = src; }

void Tex::setRenderer(SDL_Renderer *renderer) { _renderer = renderer; }

void Tex::setFrameSize(int frameSize) {
  _frame.w = frameSize;
  _frame.h = frameSize;
  _nFrame = (_width / _frame.w) * (_height / _frame.h);
}

int Tex::getFrameSize(void) const { return _frame.w; }

bool Tex::load(const std::string &src) {
  if (!_renderer)
    return false;
  if (_tex)
    unload();
  _tex = IMG_LoadTexture(_renderer, src.c_str());
  if (!_tex)
    return false;
  SDL_QueryTexture(_tex, NULL, NULL, &_width, &_height);
  return true;
}

void Tex::unload(void) {
  if (_tex)
    SDL_DestroyTexture(_tex);
  _tex = nullptr;
}

void Tex::selectFrame(int frame) {
  _currentFrame = frame;
  _frame.x = (frame % (_width / _frame.w)) * _frame.w;
  _frame.y = (frame / (_height / _frame.h)) * _frame.h;
}

void Tex::draw(int x, int y) {
  SDL_Rect dst = {x, y, _frame.w, _frame.h};
  SDL_RenderCopy(_renderer, _tex, &_frame, &dst);
}

void Tex::draw(int x, int y, int frame) {
  if (frame != _currentFrame)
    selectFrame(frame);
  draw(x, y);
}

void Tex::drawRot(int x, int y, float angle) {
  SDL_Rect dst = {x, y, _frame.w, _frame.h};
  SDL_Point center = {_frame.w / 2, _frame.h / 2};
  SDL_RenderCopyEx(_renderer, _tex, &_frame, &dst, angle, &center,
                   SDL_FLIP_NONE);
}

void Tex::drawRot(int x, int y, int frame, float angle) {
  if (frame != _currentFrame)
    selectFrame(frame);
  draw(x, y, angle);
}
