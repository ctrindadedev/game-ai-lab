#pragma once

#include <cstdint>
#include <string>


struct LevelConfiguration {
    int gridWidth = 3;
    int gridHeight = 3;
    float areaWidth = 40.0f;
    float areaHeight = 40.0f;

    float activationDistance = 10.0f;
    int maximumActiveAreas = 4;

    int enemiesPerArea = 50;
    int healthItemsPerArea = 2;
    int ammunitionItemsPerArea = 2;

    int playerHealth = 100;
    float playerSpeed = 8.0f;
    int enemyHealth = 30;
    float enemySpeed = 3.0f;
    int enemyDamagePerSecond = 10;
    int healthItemValue = 25;
    int ammunitionItemDamage = 40;
    float ammunitionItemRadius = 8.0f;

    // Distância mínima entre um inimigo recém-criado e o ponto de nascimento
    // do jogador (o centro do mundo), para que ele não nasça já encostado.
    float minimumEnemySpawnDistance = 5.0f;

    float survivalTime = 120.0f;

    std::uint32_t randomSeed = 42;

    float viewportWidth = 40.0f;
    float viewportHeight = 20.0f;
    float viewportSmoothing = 0.15f;
};

// Lê a configuração de um arquivo JSON. Retorna false em caso de falha.
bool loadLevelConfiguration(const std::string& path, LevelConfiguration& output);

// Gera uma configuração aleatória dentro de limites razoáveis
LevelConfiguration generateRandomLevelConfiguration(std::uint32_t randomSeed);
