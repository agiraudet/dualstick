#include "Text.hpp"
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

FontManager &FontManager::FM() {
  static FontManager instance;
  return instance;
}

FontManager::FontManager(void) {}

FontManager::~FontManager(void) { cleanup(); }

TTF_Font *FontManager::getFont(std::string const &fontPath, int pointSize) {
  std::string key = fontPath + std::to_string(pointSize);
  if (_fonts.find(key) == _fonts.end()) {
    TTF_Font *font = TTF_OpenFont(fontPath.c_str(), pointSize);
    _fonts[key] = font;
  }
  return _fonts[key];
}

void FontManager::cleanup(void) {
  for (auto &pair : _fonts) {
    TTF_CloseFont(pair.second);
  }
  _fonts.clear();
}

///////////////////////////////////////////////////////////////////////////////

Text::Text(void)
    : _fontPath(DEFAULT_FONT), _pointSize(DEFAUT_FONT_POINTSIZE), _text(""),
      _renderer(nullptr), _texture(nullptr), _font(nullptr) {
  _color = {255, 255, 255, 255};
  _rect = {0, 0, 0, 0};
  _font = FontManager::FM().getFont(_fontPath, _pointSize);
}

Text::Text(SDL_Renderer *renderer)
    : _fontPath(DEFAULT_FONT), _pointSize(DEFAUT_FONT_POINTSIZE), _text(""),
      _renderer(renderer), _texture(nullptr), _font(nullptr) {
  _color = {255, 255, 255, 255};
  _rect = {0, 0, 0, 0};
  _font = FontManager::FM().getFont(_fontPath, _pointSize);
}

Text::Text(Text const &other)
    : _fontPath(other._fontPath), _pointSize(other._pointSize),
      _text(other._text), _renderer(other._renderer), _font(other._font),
      _color(other._color), _rect(other._rect) {
  _generateTexture();
}

Text::~Text(void) {
  if (_texture) {
    SDL_DestroyTexture(_texture);
  }
}

Text &Text::operator=(const Text &other) {
  if (this != &other) {
    _fontPath = other._fontPath;
    _pointSize = other._pointSize;
    _text = other._text;
    _renderer = other._renderer;
    _color = other._color;
    _font = other._font;
    _rect = other._rect;
    _generateTexture();
  }
  return *this;
}

void Text::setText(std::string const &text) {
  if (_text != text) {
    _text = text;
    _generateTexture();
  }
}

void Text::setFont(std::string const &fontPath, int pointSize) {
  TTF_Font *font = FontManager::FM().getFont(fontPath, pointSize);
  if (font != _font) {
    _font = font;
    _pointSize = pointSize;
    _generateTexture();
  }
}

void Text::setPointSize(int pointSize) {
  TTF_Font *font = FontManager::FM().getFont(_fontPath, pointSize);
  if (font != _font) {
    _pointSize = pointSize;
    _font = font;
    _generateTexture();
  }
}

void Text::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  _color = {r, g, b, a};
  _generateTexture();
}

void Text::setPos(int x, int y) {
  _rect.x = x;
  _rect.y = y;
}

int Text::getWidth(void) const { return _rect.w; }

int Text::getHeight(void) const { return _rect.h; }

void Text::draw(int x, int y) {
  if (_texture) {
    _rect.x = x;
    _rect.y = y;
    SDL_RenderCopy(_renderer, _texture, nullptr, &_rect);
  }
}

void Text::draw(void) {
  if (_texture)
    SDL_RenderCopy(_renderer, _texture, nullptr, &_rect);
}

void Text::_generateTexture(void) {
  if (_texture) {
    SDL_DestroyTexture(_texture);
    _texture = nullptr;
  }
  if (_font && !_text.empty()) {
    SDL_Surface *surface = TTF_RenderText_Blended(_font, _text.c_str(), _color);
    _texture = SDL_CreateTextureFromSurface(_renderer, surface);
    _rect = {_rect.x, _rect.y, surface->w, surface->h};
    SDL_FreeSurface(surface);
  }
}
