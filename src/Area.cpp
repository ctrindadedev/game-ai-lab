#include "game/Area.h"

#include "game/Player.h"

Area::Area(int identifier, const Rectangle& bounds)
    : identifier_(identifier), bounds_(bounds) {}

void Area::updateCharacters(float /*deltaTime*/, Player& /*player*/) {
}

void Area::collectItems(Player& /*player*/) {
}

void Area::removeDeadEntities() {
}
