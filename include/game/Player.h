#pragma once

#include "game/Character.h"

class Player : public Character {
public:
    Player() = default;
    Player(Vector2 position, int health);

    void update(float deltaTime);

    // Direção de movimento desejada neste quadro, vinda do input.
    void setInputDirection(const Vector2& direction) { inputDirection_ = direction.normalized(); }
    const Vector2& inputDirection() const { return inputDirection_; }

private:
    Vector2 inputDirection_{};
};
