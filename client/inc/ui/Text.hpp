#ifndef TEXT_HPP
#define TEXT_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "Widget.hpp"

#define DEFAULT_FONT "../assets/BrassMonoRegular-d9WLg.ttf"
#define DEFAUT_FONT_POINTSIZE 12

namespace ui {

class FontManager {
public:
  static FontManager &FM(void);

  FontManager(FontManager const &) = delete;
  void operator=(FontManager const &) = delete;

  TTF_Font *getFont(std::string const &fontPath, int pointSize);
  void cleanup();

private:
  FontManager(void);
  ~FontManager(void);

private:
  std::unordered_map<std::string, TTF_Font *> _fonts;
};

class Text : public Widget {
public:
  Text(void);
  Text(std::string label);
  Text(Text const &other);
  ~Text(void);
  Text &operator=(const Text &other);

  Text &setText(std::string const &text);
  Text &setFont(std::string const &fontPath, int pointSize);
  Text &setPointSize(int pointSize);
  Widget &setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) override;
  Widget &draw(void) override;

private:
  void _generateTexture(void);

private:
  std::string _fontPath;
  int _pointSize;
  std::string _text;
  SDL_Texture *_texture;
  TTF_Font *_font;
};
} // namespace ui

#endif // !TEXT_HPP
