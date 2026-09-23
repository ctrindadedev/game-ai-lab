#pragma once

#include "game/LevelConfiguration.h"

struct BenchmarkResult {
    long long totalEnemies = 0;
    long long activeEnemies = 0;
    double averageFrameMilliseconds = 0.0;
};

// Simula frameCount quadros com o jogador parado perto de um canto do mundo
// -- o pior caso de K cheio usado nas tabelas de custo de PLAN.md -- e mede
// o tempo médio gasto atualizando inimigos, sem terminal e sem espera por
// quadro. Com dynamicActivation=false, força todas as áreas ativas o tempo
// todo: é a linha de base ingênua, O(N).
BenchmarkResult runBenchmark(const LevelConfiguration& configuration, bool dynamicActivation,
                             int frameCount);
