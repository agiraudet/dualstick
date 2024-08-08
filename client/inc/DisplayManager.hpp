#ifndef DISPLAYMANAGER_HPP
#define DISPLAYMANAGER_HPP

#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>

#include "Anim.hpp"
#include "Entity.hpp"
#include "Mob.hpp"
#include "Player.hpp"
#include "Tex.hpp"
#include "Text.hpp"

enum DMSprite { PLR_OTHER, PLR_MAIN, MOB, TILES, ANIM_SHOOT, ANIM_DAMAGE };

enum DMAnim { SHOOT, DAMAGE };

enum DMText { AMMO_CLIP, AMMO_BELT };

struct Tile {
  SDL_Rect rect;
  int frame;
};

struct DM_EntityFx {
  Entity &entity;
  Anim anim;
  float offset;
  float angleAdjust;
  DM_EntityFx(Entity &entity, Anim &anim, float offset, float angleAdjust);
  DM_EntityFx &operator=(DM_EntityFx const &other);
  void render(SDL_Rect const &camera);
};

struct DM_StaticFx {
  Anim anim;
  int x;
  int y;
  DM_StaticFx(Anim &anim, int x, int y);
  DM_StaticFx &operator=(DM_StaticFx const &other);
  void render(SDL_Rect const &camera);
};

class DisplayManager {
public:
  DisplayManager(void);
  ~DisplayManager(void);

  void generateMapTiles(Map const &map);
  void resize(int width, int height);
  void centerCamera(Entity const &entity);
  void renderFrame(Player const &player,
                   std::unordered_map<int, Player> const &otherPlayers,
                   std::unordered_map<int, std::shared_ptr<Mob>> const &mobs);
  int getCamX(void) const;
  int getCamY(void) const;
  Anim &addEntityFx(Entity &entity, DMAnim anim, float offset = 0.f,
                    float angleAdjust = 0.f);
  Anim &addStaticFx(DMAnim anim, int x, int y);
  void removeStoppedFx(void);

private:
  void _fillAtlas(void);
  void _fillAnims(void);
  void _fillGuiText(void);
  void _updateGuiText(Player const &player);
  void _renderEntity(Entity const &entity, DMSprite sprite);
  void _renderMap(void);

private:
  int _width;
  int _height;
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  SDL_Rect _camera;
  std::unordered_map<DMSprite, Tex> _atlas;
  std::unordered_map<DMAnim, Anim> _anims;
  std::unordered_map<DMText, Text> _guiTexts;
  std::vector<std::vector<Tile>> _mapTiles;
  std::vector<DM_EntityFx> _fxEntity;
  std::vector<DM_StaticFx> _fxStatic;
};

#endif
