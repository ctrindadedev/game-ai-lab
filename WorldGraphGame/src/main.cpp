#include <iostream>

#include "game/Game.h"
#include "game/LevelConfiguration.h"

int main(int argc, char** argv) {
    LevelConfiguration configuration;

    if (argc > 1) {
        if (!loadLevelConfiguration(argv[1], configuration)) {
            std::cerr << "Não foi possível ler a configuração de " << argv[1]
                      << "; usando os valores padrão.\n";
        }
    }

    std::cout << "== Game AI Lab ==\n"
              << "Malha: " << configuration.gridWidth << "x" << configuration.gridHeight
              << " áreas | máximo de ativas: " << configuration.maximumActiveAreas
              << " | sobreviver por " << configuration.survivalTime << "s\n";

    Game gameInstance(configuration);
    gameInstance.run();

    return 0;
}
