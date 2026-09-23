#include "game/LevelConfiguration.h"

#include <algorithm>
#include <fstream>
#include <random>
#include <type_traits>
#include <unordered_map>

namespace {

// Parser mínimo para o subconjunto de JSON usado nos arquivos de nível: um
// objeto plano de pares "chave": número. Não há dependência externa porque
// nenhum campo aqui é aninhado, string ou array.
std::unordered_map<std::string, double> parseFlatJsonNumbers(std::istream& input) {
    std::unordered_map<std::string, double> values;
    std::string line;
    while (std::getline(input, line)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        const auto firstQuote = line.find('"');
        if (firstQuote == std::string::npos || firstQuote > colon) {
            continue;
        }
        const auto secondQuote = line.find('"', firstQuote + 1);
        if (secondQuote == std::string::npos || secondQuote > colon) {
            continue;
        }
        const std::string key = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);

        std::string valueText = line.substr(colon + 1);
        const auto terminator = valueText.find_first_of("},");
        if (terminator != std::string::npos) {
            valueText = valueText.substr(0, terminator);
        }

        try {
            values[key] = std::stod(valueText);
        } catch (const std::exception&) {
            continue;
        }
    }
    return values;
}

bool isValid(const LevelConfiguration& configuration) {
    return configuration.gridWidth > 0 && configuration.gridHeight > 0 &&
           configuration.areaWidth > 0.0f && configuration.areaHeight > 0.0f &&
           configuration.activationDistance >= 0.0f &&
           configuration.maximumActiveAreas > 0;
}

}  // namespace

bool loadLevelConfiguration(const std::string& path, LevelConfiguration& output) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    const std::unordered_map<std::string, double> values = parseFlatJsonNumbers(file);
    const auto get = [&values](const char* key, auto& field) {
        const auto found = values.find(key);
        if (found != values.end()) {
            field = static_cast<std::decay_t<decltype(field)>>(found->second);
        }
    };

    LevelConfiguration candidate = output;
    get("gridWidth", candidate.gridWidth);
    get("gridHeight", candidate.gridHeight);
    get("areaWidth", candidate.areaWidth);
    get("areaHeight", candidate.areaHeight);
    get("activationDistance", candidate.activationDistance);
    get("maximumActiveAreas", candidate.maximumActiveAreas);
    get("enemiesPerArea", candidate.enemiesPerArea);
    get("healthItemsPerArea", candidate.healthItemsPerArea);
    get("ammunitionItemsPerArea", candidate.ammunitionItemsPerArea);
    get("playerHealth", candidate.playerHealth);
    get("playerSpeed", candidate.playerSpeed);
    get("enemyHealth", candidate.enemyHealth);
    get("enemySpeed", candidate.enemySpeed);
    get("enemyDamagePerSecond", candidate.enemyDamagePerSecond);
    get("healthItemValue", candidate.healthItemValue);
    get("ammunitionItemDamage", candidate.ammunitionItemDamage);
    get("ammunitionItemRadius", candidate.ammunitionItemRadius);
    get("minimumEnemySpawnDistance", candidate.minimumEnemySpawnDistance);
    get("survivalTime", candidate.survivalTime);
    get("randomSeed", candidate.randomSeed);
    get("viewportWidth", candidate.viewportWidth);
    get("viewportHeight", candidate.viewportHeight);
    get("viewportSmoothing", candidate.viewportSmoothing);

    if (!isValid(candidate)) {
        return false;
    }

    output = candidate;
    return true;
}

LevelConfiguration generateRandomLevelConfiguration(std::uint32_t randomSeed) {
    LevelConfiguration configuration;
    configuration.randomSeed = randomSeed;

    std::mt19937 randomEngine(randomSeed);

    std::uniform_int_distribution<int> gridSize(3, 6);
    configuration.gridWidth = gridSize(randomEngine);
    configuration.gridHeight = gridSize(randomEngine);

    std::uniform_real_distribution<float> areaDimension(20.0f, 60.0f);
    configuration.areaWidth = areaDimension(randomEngine);
    configuration.areaHeight = areaDimension(randomEngine);

    const float smallestAreaDimension = std::min(configuration.areaWidth, configuration.areaHeight);
    std::uniform_real_distribution<float> activation(5.0f, smallestAreaDimension * 0.4f);
    configuration.activationDistance = activation(randomEngine);

    std::uniform_int_distribution<int> enemiesPerArea(50, 500);
    configuration.enemiesPerArea = enemiesPerArea(randomEngine);

    std::uniform_int_distribution<int> itemsPerArea(1, 5);
    configuration.healthItemsPerArea = itemsPerArea(randomEngine);
    configuration.ammunitionItemsPerArea = itemsPerArea(randomEngine);

    std::uniform_int_distribution<int> playerHealth(80, 150);
    configuration.playerHealth = playerHealth(randomEngine);

    std::uniform_real_distribution<float> playerSpeed(6.0f, 10.0f);
    configuration.playerSpeed = playerSpeed(randomEngine);

    std::uniform_int_distribution<int> enemyHealth(15, 50);
    configuration.enemyHealth = enemyHealth(randomEngine);

    std::uniform_real_distribution<float> enemySpeed(1.5f, 4.5f);
    configuration.enemySpeed = enemySpeed(randomEngine);

    std::uniform_int_distribution<int> enemyDamage(5, 20);
    configuration.enemyDamagePerSecond = enemyDamage(randomEngine);

    std::uniform_int_distribution<int> healthValue(15, 40);
    configuration.healthItemValue = healthValue(randomEngine);

    std::uniform_int_distribution<int> ammunitionDamage(20, 60);
    configuration.ammunitionItemDamage = ammunitionDamage(randomEngine);

    std::uniform_real_distribution<float> ammunitionRadius(4.0f, 12.0f);
    configuration.ammunitionItemRadius = ammunitionRadius(randomEngine);

    std::uniform_real_distribution<float> spawnDistance(2.0f, std::max(smallestAreaDimension * 0.2f, 2.5f));
    configuration.minimumEnemySpawnDistance = spawnDistance(randomEngine);

    std::uniform_real_distribution<float> survival(60.0f, 180.0f);
    configuration.survivalTime = survival(randomEngine);

    return configuration;
}
