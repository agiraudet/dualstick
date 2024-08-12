#include "Bar.hpp"
#include "Widget.hpp"

ui::Bar::Bar(void)
    : ui::Widget(), _max(100), _current(50), _colorBg({0, 0, 0, 255}),
      _rectCurrent({0, 0, 0, 0}) {
  setColor(0xFF, 0xFF, 0xFF);
}

ui::Bar::Bar(std::string label)
    : ui::Widget(label), _max(100), _current(50), _colorBg({0, 0, 0, 255}),
      _rectCurrent({0, 0, 0, 0}) {
  setColor(0xFF, 0xFF, 0xFF);
}

ui::Bar::~Bar(void) {}

ui::Bar &ui::Bar::setMax(int max) {
  _max = max;
  _update();
  return *this;
}

ui::Bar &ui::Bar::setCurrent(int current) {
  _current = current;
  _update();
  return *this;
}

ui::Bar &ui::Bar::setSizes(int width, int height) {
  _rect.w = width;
  _rect.h = height;
  _rectCurrent.h = height;
  _update();
  return *this;
}

ui::Bar &ui::Bar::setColorBg(SDL_Color color) {
  _colorBg = color;
  return *this;
}

int ui::Bar::getMax(void) const { return _max; }

int ui::Bar::getCurrent(void) const { return _current; }

void ui::Bar::_update(void) {
  _rectCurrent.x = _rect.x;
  _rectCurrent.y = _rect.y;
  _rectCurrent.h = _rect.h;
  _rectCurrent.w = (_current * _rect.w) / _max;
}

ui::Widget &ui::Bar::draw(void) {
  SDL_SetRenderDrawColor(_renderer, _colorBg.r, _colorBg.g, _colorBg.b,
                         _colorBg.a);
  SDL_RenderFillRect(_renderer, &_rect);
  SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, _color.a);
  SDL_RenderFillRect(_renderer, &_rectCurrent);
  return *this;
}
