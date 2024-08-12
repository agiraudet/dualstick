#include "DisplayManager.hpp"
#include "Entity.hpp"
#include "Map.hpp"
#include "Text.hpp"
#include "Vector.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <memory>
#include <string>

DM_EntityFx::DM_EntityFx(Entity &entity, Anim &anim, float offset,
                         float angleAdjust)
    : entity(entity), anim(anim), offset(offset), angleAdjust(angleAdjust) {}

DM_EntityFx &DM_EntityFx::operator=(DM_EntityFx const &other) {
  if (this != &other) {
    entity = other.entity;
    anim = other.anim;
    offset = other.offset;
    angleAdjust = other.angleAdjust;
  }
  return *this;
}

void DM_EntityFx::render(SDL_Rect const &camera) {
  if (anim.isPlaying()) {
    float angle = entity.getAngle();
    float offsetX = cos(angle) * offset;
    float offsetY = sin(angle) * offset;
    float animPosX =
        entity.getPos().x + offsetX - (float)entity.getSize() / 2 - camera.x;
    float animPosY =
        entity.getPos().y + offsetY - (float)entity.getSize() / 2 - camera.y;
    anim.drawRot(animPosX, animPosY, angle * (180.0f / M_PI) + angleAdjust);
  }
}

DM_StaticFx::DM_StaticFx(Anim &anim, int x, int y) : anim(anim), x(x), y(y) {}

DM_StaticFx &DM_StaticFx::operator=(DM_StaticFx const &other) {
  if (this != &other) {
    anim = other.anim;
    x = other.x;
    y = other.y;
  }
  return *this;
}

void DM_StaticFx::render(SDL_Rect const &camera) {
  if (anim.isPlaying())
    anim.draw(x - camera.x, y - camera.y);
}

DisplayManager::DisplayManager(void)
    : _width(0), _height(0), _window(nullptr), _renderer(nullptr),
      _camera({0, 0, 0, 0}), _ui(nullptr) {
  /*resize(640 * 2, 480 * 2);*/
  resize(640, 480);
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  _window = SDL_CreateWindow("Client", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, _width, _height,
                             SDL_WINDOW_SHOWN);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  _ui.setRenderer(_renderer);
  _fillAtlas();
  _fillAnims();
  _fillGuiText();
}

DisplayManager::~DisplayManager(void) {
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void DisplayManager::generateMapTiles(Map const &map) {
  _mapTiles.clear();
  std::vector<Tile> line;
  int tileSize = map.getTileSize();
  for (int y = 0; y < map.getHeight(); y++) {
    line.clear();
    for (int x = 0; x < map.getWidth(); x++) {
      Tile tile = {{tileSize * x, tileSize * y, tileSize, tileSize},
                   map.getTile(x, y)};
      line.push_back(tile);
    }
    _mapTiles.push_back(line);
  }
}

void DisplayManager::centerCamera(Entity const &entity) {
  _camera.x = (entity.getPos().x + (float)entity.getSize() / 2) -
              (float)_camera.w / 2.f;
  _camera.y = (entity.getPos().y + (float)entity.getSize() / 2) -
              (float)_camera.h / 2.f;
}

void DisplayManager::resize(int width, int height) {
  if (width < 0 || height < 0)
    return;
  _width = width;
  _height = height;
  _camera.w = _width;
  _camera.h = _height;
  if (_window)
    SDL_SetWindowSize(_window, width, height);
}

int DisplayManager::getCamX(void) const { return _camera.x; }

int DisplayManager::getCamY(void) const { return _camera.y; }

Anim &DisplayManager::addEntityFx(Entity &entity, DMAnim anim, float offset,
                                  float angleAdjust) {
  _fxEntity.push_back(
      DM_EntityFx(entity, _anims.at(anim), offset, angleAdjust));
  return _fxEntity.back().anim;
}

Anim &DisplayManager::addStaticFx(DMAnim anim, int x, int y) {
  _fxStatic.push_back(DM_StaticFx(_anims.at(anim), x, y));
  return _fxStatic.back().anim;
}

void DisplayManager::renderFrame(int playerId, EntityManager<Player> &EMPlayer,
                                 EntityManager<Mob> &EMMob) {
  SDL_SetRenderDrawColor(_renderer, 63, 56, 81, 255);
  SDL_RenderClear(_renderer);
  _renderMap();
  for (auto const &[id, plr] : EMPlayer) {
    if (id != playerId && plr->isAlive())
      _renderEntity(*plr, PLR_OTHER);
  }
  auto player = EMPlayer.get(playerId);
  if (player && player->isAlive())
    _renderEntity(*player, PLR_MAIN);
  for (auto const &[id, mob] : EMMob)
    _renderEntity(*mob, MOB);
  for (auto &fx : _fxStatic)
    fx.render(_camera);
  for (auto &fx : _fxEntity)
    fx.render(_camera);
  if (player)
    _updateGuiText(*player);
  // for (auto &txt : _guiTexts)
  //   txt.second.draw();
  _ui.draw();
  SDL_RenderPresent(_renderer);
  removeStoppedFx();
}

void DisplayManager::_fillAtlas(void) {
  _atlas.emplace(PLR_OTHER, Tex(_renderer, "../assets/other.png"));
  _atlas.emplace(PLR_MAIN, Tex(_renderer, "../assets/player.png"));
  _atlas.emplace(MOB, Tex(_renderer, "../assets/mob.png"));
  _atlas.emplace(TILES, Tex(_renderer, "../assets/tileset.png", 32));
  _atlas.emplace(ANIM_SHOOT, Tex(_renderer, "../assets/anim.png", 16));
  _atlas.emplace(ANIM_DAMAGE, Tex(_renderer, "../assets/damage.png", 16));
  _atlas.emplace(ANIM_MOB_ATCK, Tex(_renderer, "../assets/mob_attack.png", 16));
}

void DisplayManager::_fillAnims(void) {
  _anims.emplace(SHOOT, Anim(_atlas[ANIM_SHOOT]));
  _anims.at(SHOOT).setFps(120);
  _anims.emplace(DAMAGE, Anim(_atlas[ANIM_DAMAGE]));
  _anims.at(DAMAGE).setFps(120);
  _anims.emplace(MOB_ATCK, Anim(_atlas[ANIM_MOB_ATCK]));
  _anims.at(DAMAGE).setFps(120);
}

void DisplayManager::_fillGuiText(void) {
  auto textAmmo = std::make_shared<ui::Text>("TextAmmo");
  textAmmo->setPointSize(12);
  textAmmo->setText("-");
  textAmmo->setPos(10, 440);
  _ui.addChild(textAmmo);
  auto textClip = std::make_shared<ui::Text>("TextClip");
  textClip->setPointSize(24);
  textClip->setText("-");
  textClip->setPos(10, 420);
  _ui.addChild(textClip);
  auto barHealth = std::make_shared<ui::Bar>("BarHealth");
  barHealth->setPos(10, 460);
  barHealth->setSizes(50, 10);
  barHealth->setColor(0, 0xFF, 0);
  barHealth->setMax(10);
  barHealth->setCurrent(10);
  _ui.addChild(barHealth);
  // auto textWave(std::make_shared<ui::Text>("TextWave"));
  // textWave->setPointSize(24);
  // textWave->setText("-");
  // textWave->setPos(10, 10);
  // _ui.addChild(textWave);
}

void DisplayManager::_renderEntity(Entity const &entity, DMSprite sprite) {
  _atlas.at(sprite).drawRot(
      entity.getPos().x - (float)entity.getSize() / 2 - _camera.x,
      entity.getPos().y - (float)entity.getSize() / 2 - _camera.y,
      entity.getAngle() * (180.0f / M_PI));
#ifdef DEBUG
  _renderVector(entity.getPos(), entity.getVel());
#endif // DEBUG
}

void DisplayManager::_renderMap(void) {
  for (const auto &tileLine : _mapTiles) {
    for (const auto &tile : tileLine) {
      if (SDL_HasIntersection(&_camera, &tile.rect) == SDL_TRUE) {
        _atlas.at(TILES).draw(tile.rect.x - _camera.x, tile.rect.y - _camera.y,
                              tile.frame);
      }
    }
  }
}

void DisplayManager::removeStoppedFx(void) {
  _fxEntity.erase(
      std::remove_if(_fxEntity.begin(), _fxEntity.end(),
                     [](const DM_EntityFx &fx) { return fx.anim.isStopped(); }),
      _fxEntity.end());
  _fxStatic.erase(
      std::remove_if(_fxStatic.begin(), _fxStatic.end(),
                     [](const DM_StaticFx &fx) { return fx.anim.isStopped(); }),
      _fxStatic.end());
}

void DisplayManager::_updateGuiText(Player const &player) {
  int nClip = player.weapon->clip;
  auto textClip = std::dynamic_pointer_cast<ui::Text>(_ui.getChild("TextClip"));
  textClip->setText(std::to_string(nClip));
  if (nClip <= 0)
    textClip->setColor(0xFF, 0, 0);
  else
    textClip->setColor(0xFF, 0xFF, 0xFF);
  int nAmmo = player.weapon->ammo;
  auto textAmmo = std::dynamic_pointer_cast<ui::Text>(_ui.getChild("TextAmmo"));
  textAmmo->setText(std::to_string(nAmmo));
  if (nAmmo <= 0)
    textAmmo->setColor(0xFF, 0, 0);
  else
    textAmmo->setColor(0xFF, 0xFF, 0xFF);
  std::dynamic_pointer_cast<ui::Bar>(_ui.getChild("BarHealth"))
      ->setCurrent(player.getHp());
  // std::dynamic_pointer_cast<ui::Text>(_ui.getChild("TextWave"))
  //     ->setText(std::to_string(WaveManager::WM().getWave()));
}

// DEBUG
void DisplayManager::_renderVector(Vector const &pos, Vector const &vec) {
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0x00, 0x00, 0xFF);
  int startX = pos.x - _camera.x;
  int startY = pos.y - _camera.y;
  SDL_RenderDrawLine(_renderer, startX, startY, startX + vec.x, startY + vec.y);
}
