#include "game/NonPlayerCharacter.h"

#include "game/Player.h"

NonPlayerCharacter::NonPlayerCharacter(Vector2 position, int health, float speed,
                                       int damagePerSecond)
    : Character(position, health), damagePerSecond_(damagePerSecond) {
    setSpeed(speed);
}

void NonPlayerCharacter::update(float /*deltaTime*/, Player& /*player*/) {
}

bool NonPlayerCharacter::overlaps(const Character& other) const {
    return distance(position_, other.position()) <= (radius_ + other.radius());
}
