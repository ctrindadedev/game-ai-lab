#pragma once

#include "game/Rectangle.h"
#include "game/Vector2.h"

// Câmera: sabe apenas para onde olha e qual o seu tamanho em unidades de mundo.
// Não conhece o WorldGraph nem as áreas ativas; existe somente para desenhar.
class Viewport {
public:
    Viewport() = default;
    Viewport(Vector2 center, Vector2 size);

    // Segue um alvo com suavização; smoothing em [0,1], sendo 1 instantâneo.
    void follow(const Vector2& target, float deltaTime, float smoothing = 1.0f);

    const Vector2& center() const { return center_; }
    void setCenter(const Vector2& center) { center_ = center; }

    const Vector2& size() const { return size_; }
    void setSize(const Vector2& size) { size_ = size; }

    // Retângulo do mundo atualmente visível.
    Rectangle worldBounds() const;

    bool isVisible(const Vector2& worldPosition) const;

    // Converte uma posição de mundo para coordenada relativa à câmera.
    Vector2 worldToViewport(const Vector2& worldPosition) const;

private:
    Vector2 center_{};
    Vector2 size_{40.0f, 20.0f};
};
