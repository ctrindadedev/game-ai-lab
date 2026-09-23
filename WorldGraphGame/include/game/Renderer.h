#pragma once

#include <vector>

class Viewport;
class WorldGraph;
class Player;

class Renderer {
public:
    Renderer() = default;
    Renderer(int columns, int rows);

    void draw(const WorldGraph& world, const Viewport& viewport, const Player& player);
    void drawStatusBar(const Player& player, float remainingTime,
                       const std::vector<int>& activeAreas);

private:
    int columns_ = 80;
    int rows_ = 24;
};
