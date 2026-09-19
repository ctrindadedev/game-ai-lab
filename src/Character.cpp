#include "game/Character.h"

#include <algorithm>

Character::Character(Vector2 position, int health)
    : position_(position), health_(health), maximumHealth_(health) {
    refreshDeadFlag();
}

void Character::takeDamage(int amount) {
    if (amount <= 0) {
        return;
    }
    health_ -= amount;
    refreshDeadFlag();
}

void Character::heal(int amount) {
    if (amount <= 0 || dead_) {
        return;
    }
    health_ = std::min(health_ + amount, maximumHealth_);
    refreshDeadFlag();
}

void Character::refreshDeadFlag() {
    dead_ = (health_ <= 0);
}
