#include "Mob.hpp"
#include "Entity.hpp"

int Mob::count = 0;

Mob::Mob(void) : Entity(count++) {}
