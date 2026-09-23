#pragma once

#include "game/Vector2.h"

// Base comum a Player e NonPlayerCharacter: posição, saúde e as regras de
// dano, cura e morte.
class Character {
public:
    Character() = default;
    Character(Vector2 position, int health);
    virtual ~Character() = default;

    void takeDamage(int amount);
    void heal(int amount);

    bool isDead() const { return dead_; }
    int health() const { return health_; }
    int maximumHealth() const { return maximumHealth_; }

    const Vector2& position() const { return position_; }
    void setPosition(const Vector2& position) { position_ = position; }

    float radius() const { return radius_; }
    void setRadius(float radius) { radius_ = radius; }

    float speed() const { return speed_; }
    void setSpeed(float speed) { speed_ = speed; }

protected:
    // Reavalia dead_ depois de qualquer alteração de saúde.
    void refreshDeadFlag();

    Vector2 position_{};
    int health_ = 100;
    int maximumHealth_ = 100;
    float radius_ = 0.5f;
    float speed_ = 0.0f;
    bool dead_ = false;
};
