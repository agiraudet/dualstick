#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace ui {

class Widget {
public:
  Widget(std::string label);
  Widget(Widget const &other);
  Widget &operator=(Widget const &other);
  ~Widget();

protected:
  Widget(void);

public:
  virtual Widget &draw(void) = 0;
  void drawAll(void);
  Widget &setRenderer(SDL_Renderer *renderer);
  Widget &setVisible(bool visible);
  Widget &setPos(int x, int y);
  Widget &setSize(int width, int height);
  virtual Widget &setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
  Widget &setLabel(std::string label);

  bool isVisible(void) const;
  int getX(void) const;
  int getY(void) const;
  int getWidth(void) const;
  int getHeight(void) const;
  std::string const &getLabel(void) const;

  Widget &addChild(std::shared_ptr<Widget> child);
  std::shared_ptr<Widget> getChild(std::string label);

protected:
  bool _visible;
  SDL_Renderer *_renderer;
  SDL_Rect _rect;
  SDL_Color _color;
  std::vector<std::shared_ptr<Widget>> _children;
  std::string _label;
};
} // namespace ui

#endif // !#ifndef UIWIDGET_HPP
