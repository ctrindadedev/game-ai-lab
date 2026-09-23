#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "game/Area.h"
#include "game/LevelConfiguration.h"
#include "game/Rectangle.h"
#include "game/Vector2.h"

class WorldGraph {
public:
    WorldGraph() = default;

    void build(const LevelConfiguration& configuration);

    std::size_t areaCount() const { return nodes_.size(); }

    const Rectangle& bounds() const { return bounds_; }

    const Rectangle& boundsOf(int areaIdentifier) const;
    const std::vector<int>& neighbors(int areaIdentifier) const;

    Area* area(int areaIdentifier);
    const Area* area(int areaIdentifier) const;

    Area& ensureLoaded(int areaIdentifier);
    void unload(int areaIdentifier);

    std::optional<int> areaAt(const Vector2& position) const;

    void updateActiveAreas(const Vector2& playerPosition,
                           const LevelConfiguration& configuration, float deltaTime);

    const std::vector<int>& activeAreas() const { return activeAreas_; }

private:
    struct Node {
        int identifier = -1;
        Rectangle bounds{};
        std::vector<int> neighbors;
        std::unique_ptr<Area> area;
        // Tempo acumulado desde que a área saiu do conjunto ativo; só é
        // significativo enquanto a área estiver em pendingUnload_.
        float inactiveElapsed = 0.0f;
    };

    void populate(Area& area) const;

    LevelConfiguration configuration_{};
    Rectangle bounds_{};
    std::vector<Node> nodes_;
    std::vector<int> activeAreas_;
    // Áreas carregadas, inativas, aguardando o limiar de descarte
    // (areaUnloadDelay). Tamanho limitado às áreas visitadas recentemente,
    // não ao total do mundo -- ver PLAN.md, item 7.
    std::vector<int> pendingUnload_;
};
