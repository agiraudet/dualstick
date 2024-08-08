#include "MobFactory.hpp"
#include "Message.hpp"
#include "Vector.hpp"
#include <memory>
#include <unordered_set>

MobFactory::MobFactory(void) {}

void MobFactory::createMob(mobType type, int x, int y) {
  Vector pos(x, y);
  createMob(type, pos);
}

void MobFactory::createMob(mobType type, Vector &pos) {
  std::shared_ptr<Mob> mob;
  switch (type) {
  case BASIC: {
    mob = std::make_shared<Mob>();
    break;
  }
  default:
    break;
  }
  if (mob) {
    mob->setType(type);
    mob->setPos(pos);
    _mobs[mob->getId()] = mob;
  }
}

void MobFactory::createMob(mobType type, Vector &pos, int id) {
  std::shared_ptr<Mob> mob;
  switch (type) {
  case BASIC: {
    mob = std::make_shared<Mob>();
    mob->setId(id);
    break;
  }
  default:
    break;
  }
  if (mob) {
    mob->setType(type);
    mob->setPos(pos);
    _mobs[mob->getId()] = mob;
  }
}

std::shared_ptr<Mob> MobFactory::getMobById(int id) {
  auto it = _mobs.find(id);
  if (it != _mobs.end())
    return it->second;
  else
    return nullptr;
}

void MobFactory::updateMob(int id, mobType type, Vector &pos, Vector &vel,
                           float angle) {
  if (_mobs.empty()) {
    createMob(type, pos, id);
  } else {
    auto it = _mobs.find(id);
    if (it == _mobs.end())
      createMob(type, pos, id);
  }
  std::shared_ptr mob = _mobs[id];
  mob->setPos(pos);
  mob->setVel(vel);
  mob->setAngle(angle);
  mob->setLastUpdate(std::chrono::high_resolution_clock::now());
}
int MobFactory::size(void) const { return _mobs.size(); }

void MobFactory::craftMobMsgState(MessageGameState &msg) {
  msg.nmob = _mobs.size();
  int i = 0;
  for (const auto &m : _mobs) {
    if (i >= MAX_N_MOB) {
      std::cout << msg.nmob << ": to much mobs !" << std::endl;
      break;
    }
    msg.mob[i].id = m.second->getId();
    msg.mob[i].type = m.second->getType();
    msg.mob[i].angle = m.second->getAngle();
    msg.mob[i].vel = m.second->getVel();
    msg.mob[i].pos = m.second->getPos();
    i++;
  }
}

std::unordered_map<int, std::shared_ptr<Mob>> &MobFactory::getMobs(void) {
  return _mobs;
}

void MobFactory::deleteMob(int id) { _mobs.erase(id); }

void MobFactory::removeDeadMobs(void) {
  for (auto it = _mobs.begin(); it != _mobs.end();) {
    if (it->second->getHp() <= 0) {
      it = _mobs.erase(it);
    } else {
      ++it;
    }
  }
}

void MobFactory::removeAbsentMobs(const MessageMobUpdate *updates, int nmob) {
  std::unordered_set<int> updateIds;
  for (int i = 0; i < nmob; ++i) {
    updateIds.insert(updates[i].id);
  }
  for (auto it = _mobs.begin(); it != _mobs.end();) {
    if (updateIds.find(it->first) == updateIds.end()) {
      it = _mobs.erase(it);
    } else {
      ++it;
    }
  }
}
