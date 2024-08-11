#include "Widget.hpp"
#include <memory>
#include <stdexcept>

ui::Widget::Widget()
    : _visible(true), _renderer(nullptr), _rect({0, 0, 0, 0}),
      _color({255, 255, 255, 255}), _label("none") {}

ui::Widget::Widget(std::string label)
    : _visible(true), _renderer(nullptr), _rect({0, 0, 0, 0}),
      _color({255, 255, 255, 255}), _label(label) {}

ui::Widget::Widget(Widget const &other)
    : _visible(other._visible), _renderer(other._renderer), _rect(other._rect),
      _color(other._color) {}

ui::Widget &ui::Widget::operator=(Widget const &other) {
  if (this == &other)
    return *this;
  _visible = other._visible;
  _renderer = other._renderer;
  _rect = other._rect;
  _color = other._color;
  return *this;
}

ui::Widget::~Widget() {}

void ui::Widget::drawAll(void) {
  if (_visible)
    draw();
  for (auto &child : _children) {
    child->drawAll();
  }
}

ui::Widget &ui::Widget::setRenderer(SDL_Renderer *renderer) {
  _renderer = renderer;
  return *this;
}

ui::Widget &ui::Widget::setVisible(bool visible) {
  _visible = visible;
  return *this;
}

ui::Widget &ui::Widget::setPos(int x, int y) {
  _rect.x = x;
  _rect.y = y;
  return *this;
}

ui::Widget &ui::Widget::setSize(int width, int height) {
  _rect.w = width;
  _rect.h = height;
  return *this;
}

ui::Widget &ui::Widget::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  _color.r = r;
  _color.g = g;
  _color.b = b;
  _color.a = a;
  return *this;
}

ui::Widget &ui::Widget::setLabel(std::string label) {
  _label = label;
  return *this;
}

ui::Widget &ui::Widget::addChild(std::shared_ptr<Widget> child) {
  child->setRenderer(_renderer);
  _children.push_back(child);
  return *_children.back();
}

bool ui::Widget::isVisible(void) const { return _visible; }

int ui::Widget::getX(void) const { return _rect.x; }

int ui::Widget::getY(void) const { return _rect.y; }

int ui::Widget::getWidth(void) const { return _rect.w; }

int ui::Widget::getHeight(void) const { return _rect.h; }

std::string const &ui::Widget::getLabel(void) const { return _label; }

std::shared_ptr<ui::Widget> ui::Widget::getChild(std::string label) {
  for (auto &child : _children) {
    if (child->getLabel() == label)
      return child;
    auto grandchild = child->getChild(label);
    if (grandchild)
      return grandchild;
  }
  return nullptr;
}
