#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "game/Minimax.h"
#include "game/MonteCarloTreeSearch.h"
#include "game/Players.h"
#include "game/TicTacToe.h"

namespace {

void printUsage() {
    std::cerr << "uso: tictactoe [jogadorX] [jogadorO]\n"
              << "jogadores:\n"
              << "  humano       joga pelo teclado\n"
              << "  aleatorio    jogada válida ao acaso\n"
              << "  minimax      minimax com poda alfa-beta até o fim do jogo\n"
              << "  minimax:<d>  minimax com poda alfa-beta cortado na profundidade d\n"
              << "  mcts         monte carlo tree search com 1000 iterações\n"
              << "  mcts:<N>     monte carlo tree search com N iterações\n";
}

// Converte o nome do jogador na linha de comando em um Player.
bool parsePlayer(const std::string& name, Player& player) {
    if (name == "humano") {
        player = queryPlayer;
        return true;
    }
    if (name == "aleatorio") {
        player = randomPlayer;
        return true;
    }
    if (name == "minimax") {
        player = alphabetaPlayer;
        return true;
    }
    if (name == "mcts") {
        player = mctsPlayer;
        return true;
    }

    const std::string depthPrefix = "minimax:";
    if (name.compare(0, depthPrefix.size(), depthPrefix) == 0) {
        try {
            const int d = std::stoi(name.substr(depthPrefix.size()));
            player = [d](const Game& game, const GameState& state) {
                return alphabetaCutoffSearch(state, game, d);
            };
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    const std::string iterationsPrefix = "mcts:";
    if (name.compare(0, iterationsPrefix.size(), iterationsPrefix) == 0) {
        try {
            const int N = std::stoi(name.substr(iterationsPrefix.size()));
            player = [N](const Game& game, const GameState& state) {
                return monteCarloTreeSearch(state, game, N);
            };
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string nameX = (argc > 1) ? argv[1] : "humano";
    const std::string nameO = (argc > 2) ? argv[2] : "minimax";

    Player playerX;
    Player playerO;
    if (!parsePlayer(nameX, playerX) || !parsePlayer(nameO, playerO)) {
        printUsage();
        return 1;
    }

    std::cout << "== Jogo da velha ==\n"
              << "X: " << nameX << " | O: " << nameO << "\n\n";

    const TicTacToe game;
    const int utility = game.playGame({playerX, playerO});

    if (utility > 0) {
        std::cout << "\nX venceu.\n";
    } else if (utility < 0) {
        std::cout << "\nO venceu.\n";
    } else {
        std::cout << "\nEmpate.\n";
    }
    return 0;
}
