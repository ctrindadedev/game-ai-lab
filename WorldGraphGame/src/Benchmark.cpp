#include "game/Benchmark.h"

#include <chrono>
#include <cstddef>
#include <vector>

#include "game/Area.h"
#include "game/Player.h"
#include "game/WorldGraph.h"

BenchmarkResult runBenchmark(const LevelConfiguration& configuration, bool dynamicActivation,
                             int frameCount) {
    WorldGraph world;
    world.build(configuration);

    Player player({0.0f, 0.0f}, configuration.playerHealth);
    const Rectangle& bounds = world.bounds();
    const Vector2 cornerPosition{bounds.left() + configuration.areaWidth - 0.5f,
                                 bounds.bottom() + configuration.areaHeight - 0.5f};
    player.setPosition(cornerPosition);

    constexpr float deltaTime = 1.0f / 60.0f;

    std::vector<int> allAreas;
    if (dynamicActivation) {
        world.updateActiveAreas(cornerPosition, configuration, 0.0f);
    } else {
        allAreas.reserve(world.areaCount());
        for (std::size_t identifier = 0; identifier < world.areaCount(); ++identifier) {
            world.ensureLoaded(static_cast<int>(identifier)).setState(AreaState::ACTIVE);
            allAreas.push_back(static_cast<int>(identifier));
        }
    }

    using Clock = std::chrono::steady_clock;
    const auto start = Clock::now();
    for (int frame = 0; frame < frameCount; ++frame) {
        if (dynamicActivation) {
            world.updateActiveAreas(cornerPosition, configuration, deltaTime);
        }
        const std::vector<int>& areasToUpdate = dynamicActivation ? world.activeAreas() : allAreas;
        for (int areaIdentifier : areasToUpdate) {
            if (Area* area = world.area(areaIdentifier)) {
                area->updateCharacters(deltaTime, player);
                area->collectItems(player, configuration.ammunitionItemRadius);
                area->removeDeadEntities();
            }
        }
    }
    const auto end = Clock::now();

    long long activeEnemies = 0;
    const std::vector<int>& finalAreas = dynamicActivation ? world.activeAreas() : allAreas;
    for (int areaIdentifier : finalAreas) {
        if (const Area* area = world.area(areaIdentifier)) {
            activeEnemies += static_cast<long long>(area->characters().size());
        }
    }

    BenchmarkResult result;
    result.totalEnemies = static_cast<long long>(configuration.enemiesPerArea) *
                          configuration.gridWidth * configuration.gridHeight;
    result.activeEnemies = activeEnemies;
    result.averageFrameMilliseconds =
        std::chrono::duration<double, std::milli>(end - start).count() / frameCount;
    return result;
}
