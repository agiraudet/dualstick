#include "UI.hpp"
#include "Widget.hpp"

ui::UI::UI(SDL_Renderer *renderer) : Widget() { setRenderer(renderer); }

ui::UI::~UI(void) {}

ui::Widget &ui::UI::draw(void) {
  for (auto &child : _children)
    child->drawAll();
  return *this;
}
