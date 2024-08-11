#ifndef BAR_HPP
#define BAR_HPP

#include <SDL2/SDL.h>

#include "Widget.hpp"

namespace ui {

class Bar : public Widget {
public:
  Bar(std::string label);
  ~Bar(void);

  Bar &setMax(int max);
  Bar &setCurrent(int current);
  Bar &setSizes(int width, int height);
  Bar &setColorBg(SDL_Color color);

  int getMax(void) const;
  int getCurrent(void) const;

  Widget &draw(void) override;

private:
  Bar(void);
  void _update(void);

private:
  int _max;
  int _current;
  SDL_Rect _rectCurrent;
  SDL_Color _colorBg;
};
} // namespace ui

#endif // !UIBar_HPP
