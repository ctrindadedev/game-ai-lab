#pragma once

#include <cstddef>

class Viewport;
class WorldGraph;
class Player;

// Desenho no terminal: é o único ponto que junta a Viewport e o WorldGraph.
class Renderer {
public:
    Renderer() = default;
    Renderer(int columns, int rows);

    void draw(const WorldGraph& world, const Viewport& viewport, const Player& player);
    void drawStatusBar(const Player& player, float remainingTime,
                       std::size_t activeAreaCount);

private:
    int columns_ = 80;
    int rows_ = 24;
};
