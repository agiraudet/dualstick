#include "DisplayManager.hpp"
#include "Entity.hpp"
#include <SDL2/SDL_rect.h>
#include <algorithm>

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
      _camera({0, 0, 0, 0}) {
  resize(640, 480);
  SDL_Init(SDL_INIT_VIDEO);
  _window = SDL_CreateWindow("Client", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, _width, _height,
                             SDL_WINDOW_SHOWN);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  _fillAtlas();
  _fillAnims();
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

void DisplayManager::renderFrame(
    Player const &player, std::unordered_map<int, Player> const &otherPlayers,
    std::unordered_map<int, std::shared_ptr<Mob>> const &mobs) {
  SDL_SetRenderDrawColor(_renderer, 63, 56, 81, 255);
  SDL_RenderClear(_renderer);
  _renderMap();
  for (auto const &o : otherPlayers)
    _renderEntity(o.second, PLR_OTHER);
  _renderEntity(player, PLR_MAIN);
  for (auto &m : mobs)
    _renderEntity(*m.second, MOB);
  for (auto &fx : _fxStatic)
    fx.render(_camera);
  for (auto &fx : _fxEntity)
    fx.render(_camera);
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
}

void DisplayManager::_fillAnims(void) {
  _anims.emplace(SHOOT, Anim(_atlas[ANIM_SHOOT]));
  _anims.at(SHOOT).setFps(120);
  _anims.emplace(DAMAGE, Anim(_atlas[ANIM_DAMAGE]));
  _anims.at(DAMAGE).setFps(120);
}

void DisplayManager::_renderEntity(Entity const &entity, DMSprite sprite) {
  _atlas.at(sprite).drawRot(
      entity.getPos().x - (float)entity.getSize() / 2 - _camera.x,
      entity.getPos().y - (float)entity.getSize() / 2 - _camera.y,
      entity.getAngle() * (180.0f / M_PI));
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
