#pragma once

#include <algorithm>
#include <cmath>

#include "game/Vector2.h"

// Retângulo alinhado aos eixos, definido pelo canto inferior-esquerdo e o tamanho.
struct Rectangle {
    Vector2 origin;
    Vector2 size;

    Rectangle() = default;
    Rectangle(Vector2 originValue, Vector2 sizeValue) : origin(originValue), size(sizeValue) {}

    float left()   const { return origin.x; }
    float right()  const { return origin.x + size.x; }
    float bottom() const { return origin.y; }
    float top()    const { return origin.y + size.y; }

    Vector2 center() const { return {origin.x + size.x * 0.5f, origin.y + size.y * 0.5f}; }

    bool contains(const Vector2& point) const {
        return point.x >= left() && point.x <= right() &&
               point.y >= bottom() && point.y <= top();
    }

    // Distância do ponto até a borda do retângulo; zero se o ponto estiver dentro.
    float distanceTo(const Vector2& point) const {
        const float horizontal = std::max({left() - point.x, 0.0f, point.x - right()});
        const float vertical = std::max({bottom() - point.y, 0.0f, point.y - top()});
        return std::sqrt(horizontal * horizontal + vertical * vertical);
    }

    bool intersects(const Rectangle& other) const {
        return left() <= other.right() && right() >= other.left() &&
               bottom() <= other.top() && top() >= other.bottom();
    }
};
