#include "Text.hpp"
#include "Widget.hpp"
#include <iostream>

ui::FontManager &ui::FontManager::FM() {
  static FontManager instance;
  return instance;
}

ui::FontManager::FontManager(void) {}

ui::FontManager::~FontManager(void) { cleanup(); }

TTF_Font *ui::FontManager::getFont(std::string const &fontPath, int pointSize) {
  std::string key = fontPath + std::to_string(pointSize);
  if (_fonts.find(key) == _fonts.end()) {
    TTF_Font *font = TTF_OpenFont(fontPath.c_str(), pointSize);
    _fonts[key] = font;
  }
  return _fonts[key];
}

void ui::FontManager::cleanup(void) {
  for (auto &pair : _fonts) {
    TTF_CloseFont(pair.second);
  }
  _fonts.clear();
}

///////////////////////////////////////////////////////////////////////////////

ui::Text::Text(void) : ui::Widget() {
  _texture = nullptr;
  _fontPath = DEFAULT_FONT;
  _pointSize = DEFAUT_FONT_POINTSIZE;
  _text = "";
  _font = FontManager::FM().getFont(_fontPath, _pointSize);
}

ui::Text::Text(std::string label) : ui::Widget(label) {
  _texture = nullptr;
  _fontPath = DEFAULT_FONT;
  _pointSize = DEFAUT_FONT_POINTSIZE;
  _text = "";
  _font = FontManager::FM().getFont(_fontPath, _pointSize);
}

ui::Text::Text(Text const &other) : ui::Widget(other) {
  _fontPath = other._fontPath;
  _pointSize = other._pointSize;
  _text = other._text;
  _font = other._font;
  _color = other._color;
  _generateTexture();
}

ui::Text::~Text(void) {
  if (_texture) {
    SDL_DestroyTexture(_texture);
  }
}

ui::Text &ui::Text::operator=(Text const &other) {
  if (this == &other)
    return *this;
  _renderer = other._renderer;
  _fontPath = other._fontPath;
  _pointSize = other._pointSize;
  _text = other._text;
  _font = other._font;
  _color = other._color;
  _rect = other._rect;
  _generateTexture();
  return *this;
}

ui::Text &ui::Text::setText(std::string const &text) {
  if (_text != text) {
    _text = text;
    _generateTexture();
  }
  return *this;
}

ui::Text &ui::Text::setFont(std::string const &fontPath, int pointSize) {
  TTF_Font *font = FontManager::FM().getFont(fontPath, pointSize);
  if (font != _font) {
    _font = font;
    _pointSize = pointSize;
    _generateTexture();
  }
  return *this;
}

ui::Text &ui::Text::setPointSize(int pointSize) {
  TTF_Font *font = FontManager::FM().getFont(_fontPath, pointSize);
  if (font != _font) {
    _pointSize = pointSize;
    _font = font;
    _generateTexture();
  }
  return *this;
}

ui::Widget &ui::Text::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  _color = {r, g, b, a};
  _generateTexture();
  return *this;
}

ui::Widget &ui::Text::draw(void) {
  if (_texture)
    SDL_RenderCopy(_renderer, _texture, nullptr, &_rect);
  return *this;
}

void ui::Text::_generateTexture(void) {
  if (_renderer && _font && !_text.empty()) {
    if (_texture) {
      SDL_DestroyTexture(_texture);
      _texture = nullptr;
    }
    SDL_Surface *surface = TTF_RenderText_Blended(_font, _text.c_str(), _color);
    _texture = SDL_CreateTextureFromSurface(_renderer, surface);
    _rect = {_rect.x, _rect.y, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }
}
