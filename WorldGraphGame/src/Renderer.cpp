#include "game/Renderer.h"

#include <iostream>

#include "game/Player.h"
#include "game/Viewport.h"
#include "game/WorldGraph.h"

Renderer::Renderer(int columns, int rows) : columns_(columns), rows_(rows) {}

void Renderer::draw(const WorldGraph& /*world*/, const Viewport& /*viewport*/,
                    const Player& /*player*/) {
}

void Renderer::drawStatusBar(const Player& player, float remainingTime,
                             std::size_t activeAreaCount) {
    std::cout << "Saúde: " << player.health() << "/" << player.maximumHealth()
              << " | tempo restante: " << remainingTime << "s"
              << " | áreas ativas: " << activeAreaCount << '\n';
}
