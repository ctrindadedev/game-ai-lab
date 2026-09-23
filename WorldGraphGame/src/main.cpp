#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "game/Game.h"
#include "game/LevelConfiguration.h"

namespace {

// --random <semente>: gera o nível aleatoriamente. Sem --random, o único
// argumento posicional é o caminho de um arquivo de nível em JSON.
LevelConfiguration parseArguments(int argc, char** argv) {
    LevelConfiguration configuration;

    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];

        if (argument == "--random") {
            if (index + 1 >= argc) {
                std::cerr << "--random exige uma semente; usando os valores padrão.\n";
                continue;
            }
            try {
                const std::uint32_t seed = static_cast<std::uint32_t>(std::stoul(argv[++index]));
                configuration = generateRandomLevelConfiguration(seed);
            } catch (const std::exception&) {
                std::cerr << "Semente inválida em --random; usando os valores padrão.\n";
            }
            continue;
        }

        if (!loadLevelConfiguration(argument, configuration)) {
            std::cerr << "Não foi possível ler a configuração de " << argument
                      << "; usando os valores padrão.\n";
        }
    }

    return configuration;
}

}  // namespace

int main(int argc, char** argv) {
    const LevelConfiguration configuration = parseArguments(argc, argv);

    std::cout << "== Game AI Lab ==\n"
              << "Malha: " << configuration.gridWidth << "x" << configuration.gridHeight
              << " áreas | máximo de ativas: " << configuration.maximumActiveAreas
              << " | sobreviver por " << configuration.survivalTime << "s\n";

    Game gameInstance(configuration);
    gameInstance.run();

    return 0;
}
