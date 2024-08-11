#ifndef UI_HPP
#define UI_HPP

#include "Bar.hpp"
#include "Text.hpp"
#include "Widget.hpp"

namespace ui {
class UI : public Widget {
public:
  UI(SDL_Renderer *renderer);
  ~UI(void);
  Widget &draw(void) override;
};
} // namespace ui

#endif // !#ifndef UI_HPP
