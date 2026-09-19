#pragma once

#include "game/Vector2.h"

enum class ItemType {
    HEALTH,
    AMMUNITION
};

// Coletável posicionado dentro de uma Area.
struct Item {
    Vector2 position{};
    ItemType type = ItemType::HEALTH;
    int value = 0;
    float radius = 0.5f;
    bool collected = false;
};
