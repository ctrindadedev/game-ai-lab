#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "game/Benchmark.h"
#include "game/Game.h"
#include "game/LevelConfiguration.h"

namespace {

void runBenchmarkSuite(const std::string& outputPath) {
    struct MeshSpec {
        int width;
        int height;
        const char* label;
    };

    const std::vector<int> enemiesPerAreaSweep{10, 50, 100, 500, 1000};
    const std::vector<MeshSpec> meshes{{3, 3, "3x3"}, {10, 10, "10x10"}};
    constexpr int frameCount = 60;

    const std::filesystem::path path(outputPath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream csv(outputPath);
    csv << "malha,enemiesPerArea,totalEnemies,modo,averageFrameMilliseconds\n";

    std::cout << std::left << std::setw(8) << "malha" << std::setw(14) << "totalInimigos"
              << std::setw(10) << "modo" << "tempo medio de quadro (ms)\n";

    for (const MeshSpec& mesh : meshes) {
        for (int enemiesPerArea : enemiesPerAreaSweep) {
            LevelConfiguration configuration;
            configuration.gridWidth = mesh.width;
            configuration.gridHeight = mesh.height;
            configuration.enemiesPerArea = enemiesPerArea;

            for (bool dynamicActivation : {true, false}) {
                const BenchmarkResult result =
                    runBenchmark(configuration, dynamicActivation, frameCount);
                const char* modeLabel = dynamicActivation ? "janela" : "ingenuo";

                csv << mesh.label << ',' << enemiesPerArea << ',' << result.totalEnemies << ','
                    << modeLabel << ',' << result.averageFrameMilliseconds << '\n';

                std::cout << std::left << std::setw(8) << mesh.label << std::setw(14)
                          << result.totalEnemies << std::setw(10) << modeLabel << std::fixed
                          << std::setprecision(4) << result.averageFrameMilliseconds << '\n';
            }
        }
    }

    std::cout << "\nCSV salvo em " << outputPath << '\n';
}

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
    if (argc > 1 && std::string(argv[1]) == "--benchmark") {
        const std::string outputPath = (argc > 2) ? argv[2] : "benchmarks/frame_time.csv";
        runBenchmarkSuite(outputPath);
        return 0;
    }

    const LevelConfiguration configuration = parseArguments(argc, argv);

    std::cout << "== Game AI Lab ==\n"
              << "Malha: " << configuration.gridWidth << "x" << configuration.gridHeight
              << " áreas | máximo de ativas: " << configuration.maximumActiveAreas
              << " | sobreviver por " << configuration.survivalTime << "s\n";

    Game gameInstance(configuration);
    gameInstance.run();

    return 0;
}
