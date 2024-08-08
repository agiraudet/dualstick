#ifndef MOBFACTORY_HPP
#define MOBFACTORY_HPP

#include "Message.hpp"
#include "Mob.hpp"
#include "Vector.hpp"
#include <memory>
#include <unordered_map>

class MobFactory {
public:
  MobFactory(void);

  void createMob(mobType, int x, int y);
  void createMob(mobType, Vector &pos);
  void createMob(mobType, Vector &pos, int id);
  std::shared_ptr<Mob> getMobById(int id);
  void updateMob(int id, mobType type, Vector &pos, Vector &vel, float angle);
  void craftMobMsgState(MessageGameState &msg);
  int size(void) const;
  std::unordered_map<int, std::shared_ptr<Mob>> &getMobs(void);
  void deleteMob(int id);
  void removeDeadMobs(void);
  void removeAbsentMobs(const MessageMobUpdate *updates, int nmob);

private:
  std::unordered_map<int, std::shared_ptr<Mob>> _mobs;
};

#endif
