#include "game/WorldGraph.h"

#include "game/LevelConfiguration.h"

void WorldGraph::build(const LevelConfiguration& configuration) {
    nodes_.clear();
    activeAreas_.clear();

    const int columns = configuration.gridWidth;
    const int rows = configuration.gridHeight;
    nodes_.resize(static_cast<std::size_t>(columns * rows));

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

            if (column > 0)           node.neighbors.push_back(indexOf(column - 1, row));
            if (column < columns - 1) node.neighbors.push_back(indexOf(column + 1, row));
            if (row > 0)              node.neighbors.push_back(indexOf(column, row - 1));
            if (row < rows - 1)       node.neighbors.push_back(indexOf(column, row + 1));
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
    }
    return *node.area;
}

void WorldGraph::unload(int areaIdentifier) {
    nodes_.at(static_cast<std::size_t>(areaIdentifier)).area.reset();
}

std::optional<int> WorldGraph::areaAt(const Vector2& position) const {
    for (const Node& node : nodes_) {
        if (node.bounds.contains(position)) {
            return node.identifier;
        }
    }
    return std::nullopt;
}

void WorldGraph::updateActiveAreas(const Vector2& /*playerPosition*/,
                                   const LevelConfiguration& /*configuration*/) {
}
