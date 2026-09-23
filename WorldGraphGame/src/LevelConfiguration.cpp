#include "game/LevelConfiguration.h"

#include <random>

bool loadLevelConfiguration(const std::string& /*path*/, LevelConfiguration& /*output*/) {
    return false;
}

LevelConfiguration generateRandomLevelConfiguration(std::uint32_t randomSeed) {
    LevelConfiguration configuration;
    configuration.randomSeed = randomSeed;

    std::mt19937 randomEngine(randomSeed);
    (void)randomEngine;

    return configuration;
}
