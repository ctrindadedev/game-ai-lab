#pragma once

#include "game/Character.h"

class Player;

// Inimigo simples: persegue o jogador e causa dano por contato.
class NonPlayerCharacter : public Character {
public:
    NonPlayerCharacter() = default;
    NonPlayerCharacter(Vector2 position, int health, float speed, int damagePerSecond);

    // Move em direção ao jogador e aplica dano se houver sobreposição.
    void update(float deltaTime, Player& player);

    int damagePerSecond() const { return damagePerSecond_; }
    void setDamagePerSecond(int damagePerSecond) { damagePerSecond_ = damagePerSecond; }

private:
    bool overlaps(const Character& other) const;

    int damagePerSecond_ = 10;
    float damageAccumulator_ = 0.0f;
};
