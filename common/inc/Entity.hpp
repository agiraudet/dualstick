#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Message.hpp"
#include "Vector.hpp"
#include "Weapon.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#define ENTITY_MAXSPEED 400.f
#define ENTITY_SIZE 20

class Map;

class Entity {
protected:
  Entity(void);
  Entity(int id);

public:
  virtual ~Entity(void) = 0;

  void setId(int id);
  void setPos(Vector &pos);
  void setVel(Vector &vel);
  void setAngle(float angle);
  void setMaxSpeed(float maxSpeed);
  void
  setLastUpdate(std::chrono::high_resolution_clock::time_point currentTime);
  void setHp(int hp);
  void setAlive(bool alive);

  int getId(void) const;
  Vector const &getPos(void) const;
  Vector const &getVel(void) const;
  float getAngle(void) const;
  float getMaxSpeed(void) const;
  int getSize(void) const;
  int getHp(void) const;
  bool isAlive(void) const;

  void update(Vector const &pos, Vector const &vel, float angle, int hp);
  void move(void);
  void move(Map &map);
  void aimAngle(int targetX, int targetY);
  void capSpeed(void);
  void capSpeed(int speed);
  void injure(int damage);

public:
  Weapon *weapon;

protected:
  bool _alive;
  int _id;
  Vector _velocity;
  Vector _position;
  float _angle;
  float _maxSpeed;
  int _size;
  int _hp;
  std::chrono::high_resolution_clock::time_point _lastMove;
};

template <typename T> class EntityManager {
public:
  using EntityID = int;
  using EntityPtr = std::shared_ptr<T>;

public:
  EntityID create(void) {
    static EntityID nextID = 0;
    _entities.emplace(nextID, std::make_shared<T>(nextID));
    return nextID++;
  }

  EntityPtr create(EntityID id) {
    auto it = _entities.find(id);
    if (it == _entities.end()) {
      _entities.emplace(id, std::make_shared<T>(id));
      return _entities[id];
    } else {
      return it->second;
    }
  }

  EntityPtr get(EntityID id) const {
    auto it = _entities.find(id);
    if (it != _entities.end())
      return it->second;
    else
      return nullptr;
  }

  void remove(EntityID id) {
    auto it = _entities.find(id);
    if (it != _entities.end())
      _entities.erase(it);
  }

  void fillMsg(int &nEntity, MessageEntityUpdate *msg) {
    int max = nEntity;
    nEntity = _entities.size();
    int i = 0;
    for (auto &[id, entity] : _entities) {
      if (i >= max) {
        std::cerr << "[EntityManager] To much entities for message!"
                  << std::endl;
        break;
      }
      msg[i].id = entity->getId();
      msg[i].angle = entity->getAngle();
      msg[i].vel = entity->getVel();
      msg[i].pos = entity->getPos();
      msg[i].hp = entity->getHp();
      i++;
    }
  }

  void removeDeads(void) {
    for (auto it = _entities.begin(); it != _entities.end();) {
      if (!it->second->isAlive())
        it = _entities.erase(it);
      else
        ++it;
    }
  }

  void removeIfNotInSet(const std::unordered_set<EntityID> &idSet) {
    for (auto it = _entities.begin(); it != _entities.end();) {
      if (idSet.find(it->first) == idSet.end()) {
        it = _entities.erase(it);
      } else {
        ++it;
      }
    }
  }

  int size(void) const { return _entities.size(); }

  bool empty(void) const { return _entities.empty(); }

  auto begin() const { return _entities.cbegin(); }
  auto end() const { return _entities.cend(); }
  auto begin() { return _entities.begin(); }
  auto end() { return _entities.end(); }

private:
  std::unordered_map<EntityID, EntityPtr> _entities;
};

#endif
