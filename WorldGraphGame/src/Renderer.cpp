#include "game/Renderer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "game/Area.h"
#include "game/Player.h"
#include "game/Viewport.h"
#include "game/WorldGraph.h"

Renderer::Renderer(int columns, int rows) : columns_(columns), rows_(rows) {}

void Renderer::draw(const WorldGraph& world, const Viewport& viewport, const Player& player) {
    // A última linha do quadro é reservada para a barra de status.
    const int gridRows = std::max(rows_ - 1, 1);
    std::vector<char> grid(static_cast<std::size_t>(columns_) * static_cast<std::size_t>(gridRows), ' ');

    const float cellWidth = viewport.size().x / static_cast<float>(columns_);
    const float cellHeight = viewport.size().y / static_cast<float>(gridRows);

    const auto plot = [&](const Vector2& worldPosition, char glyph) {
        if (!viewport.isVisible(worldPosition)) {
            return;
        }
        const Vector2 local = viewport.worldToViewport(worldPosition);
        const int column = std::clamp(static_cast<int>(local.x / cellWidth), 0, columns_ - 1);
        const int worldRow = std::clamp(static_cast<int>(local.y / cellHeight), 0, gridRows - 1);
        const int row = gridRows - 1 - worldRow;  // y de mundo cresce para cima; linha, para baixo.
        grid[static_cast<std::size_t>(row * columns_ + column)] = glyph;
    };

    const Rectangle visible = viewport.worldBounds();

    // Fundo fixo no mundo: bordas das áreas e uma grade de pontos. Sem isso a
    // câmera, que segue o jogador, deixa o '@' parado no centro e não dá para
    // ver o deslocamento.
    constexpr float dotSpacing = 5.0f;
    const Rectangle& worldBounds = world.bounds();
    const Vector2 areaSize = world.areaCount() > 0 ? world.boundsOf(0).size : worldBounds.size;

    // Há uma linha (múltiplo de spacing, contado a partir de origin) no intervalo (start, end]?
    const auto crossesLine = [](float start, float end, float origin, float spacing) {
        return std::floor((end - origin) / spacing) != std::floor((start - origin) / spacing);
    };

    for (int row = 0; row < gridRows; ++row) {
        const float cellBottom = visible.bottom() + static_cast<float>(gridRows - 1 - row) * cellHeight;
        for (int column = 0; column < columns_; ++column) {
            const float cellLeft = visible.left() + static_cast<float>(column) * cellWidth;
            const Rectangle cell{{cellLeft, cellBottom}, {cellWidth, cellHeight}};
            if (!cell.intersects(worldBounds)) {
                continue;
            }

            const float cellRight = cell.right();
            const float cellTop = cell.top();
            const bool verticalBorder =
                crossesLine(cellLeft, cellRight, worldBounds.left(), areaSize.x);
            const bool horizontalBorder =
                crossesLine(cellBottom, cellTop, worldBounds.bottom(), areaSize.y);

            char glyph = ' ';
            if (verticalBorder && horizontalBorder) {
                glyph = '+';
            } else if (verticalBorder) {
                glyph = '|';
            } else if (horizontalBorder) {
                glyph = '-';
            } else if (crossesLine(cellLeft, cellRight, worldBounds.left(), dotSpacing) &&
                       crossesLine(cellBottom, cellTop, worldBounds.bottom(), dotSpacing)) {
                glyph = '.';
            }
            grid[static_cast<std::size_t>(row * columns_ + column)] = glyph;
        }
    }

    for (int areaIdentifier : world.activeAreas()) {
        const Area* area = world.area(areaIdentifier);
        if (!area || !area->bounds().intersects(visible)) {
            continue;
        }

        for (const Item& item : area->items()) {
            if (!item.collected) {
                plot(item.position, item.type == ItemType::HEALTH ? 'h' : 'a');
            }
        }
        for (const NonPlayerCharacter& enemy : area->characters()) {
            if (!enemy.isDead()) {
                plot(enemy.position(), 'e');
            }
        }
    }

    plot(player.position(), '@');

    std::string frame = "\033[H";
    for (int row = 0; row < gridRows; ++row) {
        frame.append(grid.data() + static_cast<std::size_t>(row) * static_cast<std::size_t>(columns_),
                     static_cast<std::size_t>(columns_));
        frame += "\033[K\n";
    }
    frameBuffer_ = std::move(frame);
}

void Renderer::drawStatusBar(const Player& player, float remainingTime,
                             const std::vector<int>& activeAreas) {
    std::string activeList;
    for (int areaIdentifier : activeAreas) {
        activeList += (activeList.empty() ? "" : " ") + std::to_string(areaIdentifier);
    }

    std::ostringstream status;
    status << std::fixed << std::setprecision(1)
           << "Saúde: " << player.health() << "/" << player.maximumHealth()
           << " | tempo: " << remainingTime << "s"
           << " | posição: (" << player.position().x << ", " << player.position().y << ")"
           << " | áreas ativas (" << activeAreas.size() << "): " << activeList;

    frameBuffer_ += status.str();
    frameBuffer_ += "\033[K";

    std::cout << frameBuffer_ << std::flush;
    frameBuffer_.clear();
}
