#include "game/Viewport.h"

Viewport::Viewport(Vector2 center, Vector2 size) : center_(center), size_(size) {}

void Viewport::follow(const Vector2& /*target*/, float /*deltaTime*/, float /*smoothing*/) {
}

Rectangle Viewport::worldBounds() const {
    return Rectangle{{center_.x - size_.x * 0.5f, center_.y - size_.y * 0.5f}, size_};
}

bool Viewport::isVisible(const Vector2& worldPosition) const {
    return worldBounds().contains(worldPosition);
}

Vector2 Viewport::worldToViewport(const Vector2& worldPosition) const {
    const Rectangle bounds = worldBounds();
    return {worldPosition.x - bounds.left(), worldPosition.y - bounds.bottom()};
}
