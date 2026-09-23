#include "game/WorldGraph.h"

#include <algorithm>
#include <random>

void WorldGraph::build(const LevelConfiguration& configuration) {
    configuration_ = configuration;
    nodes_.clear();
    activeAreas_.clear();

    const int columns = configuration.gridWidth;
    const int rows = configuration.gridHeight;
    nodes_.resize(static_cast<std::size_t>(columns * rows));
    bounds_ = Rectangle{{0.0f, 0.0f},
                        {static_cast<float>(columns) * configuration.areaWidth,
                         static_cast<float>(rows) * configuration.areaHeight}};

    const auto indexOf = [columns](int column, int row) { return row * columns + column; };

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            const int identifier = indexOf(column, row);
            Node& node = nodes_[static_cast<std::size_t>(identifier)];
            node.identifier = identifier;
            node.bounds = Rectangle{
                {static_cast<float>(column) * configuration.areaWidth,
                 static_cast<float>(row) * configuration.areaHeight},
                {configuration.areaWidth, configuration.areaHeight}};

            for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
                for (int columnOffset = -1; columnOffset <= 1; ++columnOffset) {
                    const int neighborColumn = column + columnOffset;
                    const int neighborRow = row + rowOffset;
                    const bool isSelf = (rowOffset == 0 && columnOffset == 0);
                    const bool isInside = neighborColumn >= 0 && neighborColumn < columns &&
                                          neighborRow >= 0 && neighborRow < rows;
                    if (!isSelf && isInside) {
                        node.neighbors.push_back(indexOf(neighborColumn, neighborRow));
                    }
                }
            }
        }
    }
}

const Rectangle& WorldGraph::boundsOf(int areaIdentifier) const {
    return nodes_.at(static_cast<std::size_t>(areaIdentifier)).bounds;
}

const std::vector<int>& WorldGraph::neighbors(int areaIdentifier) const {
    return nodes_.at(static_cast<std::size_t>(areaIdentifier)).neighbors;
}

Area* WorldGraph::area(int areaIdentifier) {
    return nodes_.at(static_cast<std::size_t>(areaIdentifier)).area.get();
}

const Area* WorldGraph::area(int areaIdentifier) const {
    return nodes_.at(static_cast<std::size_t>(areaIdentifier)).area.get();
}

Area& WorldGraph::ensureLoaded(int areaIdentifier) {
    Node& node = nodes_.at(static_cast<std::size_t>(areaIdentifier));
    if (!node.area) {
        node.area = std::make_unique<Area>(node.identifier, node.bounds);
        populate(*node.area);
    }
    return *node.area;
}

void WorldGraph::unload(int areaIdentifier) {
    nodes_.at(static_cast<std::size_t>(areaIdentifier)).area.reset();
}

void WorldGraph::populate(Area& area) const {
    std::seed_seq seed{configuration_.randomSeed,
                       static_cast<std::uint32_t>(area.identifier())};
    std::mt19937 randomEngine(seed);

    const Rectangle& bounds = area.bounds();
    std::uniform_real_distribution<float> horizontal(bounds.left(), bounds.right());
    std::uniform_real_distribution<float> vertical(bounds.bottom(), bounds.top());
    const auto randomPosition = [&] { return Vector2{horizontal(randomEngine), vertical(randomEngine)}; };

    // O jogador nasce no centro do mundo; evita que um inimigo nasça
    // encostado nele. Poucas tentativas bastam, e aceitar a última é
    // preferível a travar a geração do nível.
    const Vector2 playerSpawnPoint = bounds_.center();
    const auto randomEnemyPosition = [&] {
        Vector2 candidate = randomPosition();
        for (int attempt = 0; attempt < 20 &&
                              distance(candidate, playerSpawnPoint) < configuration_.minimumEnemySpawnDistance;
             ++attempt) {
            candidate = randomPosition();
        }
        return candidate;
    };

    area.characters().reserve(static_cast<std::size_t>(configuration_.enemiesPerArea));
    for (int count = 0; count < configuration_.enemiesPerArea; ++count) {
        area.characters().emplace_back(randomEnemyPosition(), configuration_.enemyHealth,
                                       configuration_.enemySpeed,
                                       configuration_.enemyDamagePerSecond);
    }

    for (int count = 0; count < configuration_.healthItemsPerArea; ++count) {
        Item item;
        item.position = randomPosition();
        item.type = ItemType::HEALTH;
        item.value = configuration_.healthItemValue;
        area.items().push_back(item);
    }

    for (int count = 0; count < configuration_.ammunitionItemsPerArea; ++count) {
        Item item;
        item.position = randomPosition();
        item.type = ItemType::AMMUNITION;
        item.value = configuration_.ammunitionItemDamage;
        area.items().push_back(item);
    }
}

std::optional<int> WorldGraph::areaAt(const Vector2& position) const {
    for (const Node& node : nodes_) {
        if (node.bounds.contains(position)) {
            return node.identifier;
        }
    }
    return std::nullopt;
}

void WorldGraph::updateActiveAreas(const Vector2& playerPosition,
                                   const LevelConfiguration& configuration) {
    const std::optional<int> current = areaAt(playerPosition);
    if (!current) {
        return;
    }

    std::vector<int> candidates{*current};
    for (int neighbor : neighbors(*current)) {
        if (boundsOf(neighbor).distanceTo(playerPosition) < configuration.activationDistance) {
            candidates.push_back(neighbor);
        }
    }

    const std::size_t limit = static_cast<std::size_t>(std::max(configuration.maximumActiveAreas, 1));
    if (candidates.size() > limit) {
        std::sort(candidates.begin() + 1, candidates.end(), [&](int first, int second) {
            return boundsOf(first).distanceTo(playerPosition) <
                   boundsOf(second).distanceTo(playerPosition);
        });
        candidates.resize(limit);
    }

    for (int areaIdentifier : activeAreas_) {
        const bool stillActive =
            std::find(candidates.begin(), candidates.end(), areaIdentifier) != candidates.end();
        if (!stillActive) {
            if (Area* inactiveArea = area(areaIdentifier)) {
                inactiveArea->setState(AreaState::INACTIVE);
            }
        }
    }

    for (int areaIdentifier : candidates) {
        ensureLoaded(areaIdentifier).setState(AreaState::ACTIVE);
    }

    activeAreas_ = std::move(candidates);
}
