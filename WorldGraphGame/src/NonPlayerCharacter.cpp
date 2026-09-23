#include "game/NonPlayerCharacter.h"

#include "game/Player.h"

NonPlayerCharacter::NonPlayerCharacter(Vector2 position, int health, float speed,
                                       int damagePerSecond)
    : Character(position, health), damagePerSecond_(damagePerSecond) {
    setSpeed(speed);
}

void NonPlayerCharacter::update(float deltaTime, Player& player) {
    const Vector2 toPlayer = player.position() - position_;
    position_ += toPlayer.normalized() * speed_ * deltaTime;

    if (!overlaps(player)) {
        return;
    }

    damageAccumulator_ += static_cast<float>(damagePerSecond_) * deltaTime;
    if (damageAccumulator_ >= 1.0f) {
        const int wholeDamage = static_cast<int>(damageAccumulator_);
        player.takeDamage(wholeDamage);
        damageAccumulator_ -= static_cast<float>(wholeDamage);
    }
}

bool NonPlayerCharacter::overlaps(const Character& other) const {
    return distance(position_, other.position()) <= (radius_ + other.radius());
}
