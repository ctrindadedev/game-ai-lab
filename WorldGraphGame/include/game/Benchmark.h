#pragma once

#include "game/LevelConfiguration.h"

struct BenchmarkResult {
    long long totalEnemies = 0;
    long long activeEnemies = 0;
    double averageFrameMilliseconds = 0.0;
};

BenchmarkResult runBenchmark(const LevelConfiguration& configuration, bool dynamicActivation,
                             int frameCount);
