#ifndef TEXT_HPP
#define TEXT_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <string>
#include <unordered_map>

#define DEFAULT_FONT "../assets/BrassMonoRegular-d9WLg.ttf"
#define DEFAUT_FONT_POINTSIZE 12

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

class Text {
public:
  Text(SDL_Renderer *renderer);
  Text(Text const &other);
  ~Text(void);
  Text &operator=(const Text &other);

  void setText(std::string const &text);
  void setFont(std::string const &fontPath, int pointSize);
  void setPointSize(int pointSize);
  void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
  void setPos(int x, int y);
  int getWidth(void) const;
  int getHeight(void) const;
  void draw(int x, int y);
  void draw(void);

private:
  Text(void);
  void _generateTexture(void);

private:
  std::string _fontPath;
  int _pointSize;
  std::string _text;
  SDL_Renderer *_renderer;
  SDL_Texture *_texture;
  SDL_Color _color;
  TTF_Font *_font;
  SDL_Rect _rect;
};

#endif // !TEXT_HPP
