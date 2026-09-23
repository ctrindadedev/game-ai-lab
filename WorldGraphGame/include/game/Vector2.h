#pragma once

#include <cmath>

// Vetor 2D em unidades de mundo. Usado para posição, direção e tamanho.
struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float xValue, float yValue) : x(xValue), y(yValue) {}

    Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
    Vector2 operator-(const Vector2& other) const { return {x - other.x, y - other.y}; }
    Vector2 operator*(float scalar) const { return {x * scalar, y * scalar}; }

    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }

    float length() const { return std::sqrt(x * x + y * y); }

    // Retorna o vetor normalizado, ou (0,0) se o comprimento for desprezível.
    Vector2 normalized() const {
        const float currentLength = length();
        return (currentLength > 1e-6f) ? Vector2{x / currentLength, y / currentLength}
                                       : Vector2{0.0f, 0.0f};
    }
};

inline float distance(const Vector2& first, const Vector2& second) {
    return (first - second).length();
}

// Interpolação linear entre dois pontos; factor em [0,1].
inline Vector2 lerp(const Vector2& from, const Vector2& to, float factor) {
    return from + (to - from) * factor;
}
