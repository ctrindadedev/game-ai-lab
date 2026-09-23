#include "game/Renderer.h"

#include <iomanip>
#include <iostream>
#include <string>

#include "game/Player.h"
#include "game/Viewport.h"
#include "game/WorldGraph.h"

Renderer::Renderer(int columns, int rows) : columns_(columns), rows_(rows) {}

void Renderer::draw(const WorldGraph&, const Viewport&,
                    const Player&) {
}

void Renderer::drawStatusBar(const Player& player, float remainingTime,
                             const std::vector<int>& activeAreas) {
    std::string activeList;
    for (int areaIdentifier : activeAreas) {
        activeList += (activeList.empty() ? "" : " ") + std::to_string(areaIdentifier);
    }

    std::cout << '\r' << std::fixed << std::setprecision(1)
              << "Saúde: " << player.health() << "/" << player.maximumHealth()
              << " | tempo: " << remainingTime << "s"
              << " | posição: (" << player.position().x << ", " << player.position().y << ")"
              << " | áreas ativas (" << activeAreas.size() << "): " << activeList
              << "\033[K" << std::flush;
}
