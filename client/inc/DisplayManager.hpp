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
#include "UI.hpp"
#include "Vector.hpp"

enum DMSprite {
  PLR_OTHER,
  PLR_MAIN,
  MOB,
  TILES,
  SHOP,
  ANIM_SHOOT,
  ANIM_DAMAGE,
  ANIM_MOB_ATCK
};

enum DMAnim { SHOOT, DAMAGE, MOB_ATCK };

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
  void generateShopTiles(Map const &map);
  void resize(int width, int height);
  void centerCamera(Entity const &entity);
  void renderFrame(int playerId, EntityManager<Player> &EMPlayer,
                   EntityManager<Mob> &EMMob);
  int getCamX(void) const;
  int getCamY(void) const;
  Anim &addEntityFx(Entity &entity, DMAnim anim, float offset = 0.f,
                    float angleAdjust = 0.f);
  Anim &addStaticFx(DMAnim anim, int x, int y);
  void removeStoppedFx(void);
  void updateWaveNumber(int wave);

private:
  void _fillAtlas(void);
  void _fillAnims(void);
  void _fillGui(void);
  void _updateGui(Player const &player);
  void _renderEntity(Entity const &entity, DMSprite sprite,
                     bool rotatedByVelocity = false);
  void _renderMap(void);
  void _renderShop(void);

  // DEBUG
private:
  void _renderVector(Vector const &pos, Vector const &vec);

private:
  int _width;
  int _height;
  SDL_Window *_window;
  SDL_Renderer *_renderer;
  SDL_Rect _camera;
  ui::UI _ui;
  std::unordered_map<DMSprite, Tex> _atlas;
  std::unordered_map<DMAnim, Anim> _anims;
  std::vector<std::vector<Tile>> _mapTiles;
  std::vector<Tile> _shopTiles;
  std::vector<DM_EntityFx> _fxEntity;
  std::vector<DM_StaticFx> _fxStatic;
};

#endif
