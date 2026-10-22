#include "game/Players.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "game/Minimax.h"
#include "game/MonteCarloTreeSearch.h"

Move queryPlayer(const Game& game, const GameState& state) {
    std::cout << "estado atual:\n";
    game.display(state);

    const std::vector<Move> moves = game.actions(state);
    std::cout << "jogadas disponíveis:";
    for (const Move& move : moves) {
        std::cout << " (" << move.first << ", " << move.second << ")";
    }
    std::cout << "\n\n";

    // Diferente do original, pede de novo em vez de aceitar uma jogada
    // inválida, que passaria a vez sem mudar o estado.
    std::string line;
    while (std::cout << "Sua jogada? " && std::getline(std::cin, line)) {
        std::replace_if(line.begin(), line.end(),
                        [](char c) { return c == '(' || c == ')' || c == ','; }, ' ');
        std::istringstream input(line);
        Move move;
        if (input >> move.first >> move.second &&
            std::find(moves.begin(), moves.end(), move) != moves.end()) {
            return move;
        }
        std::cout << "jogada inválida, digite linha e coluna, por exemplo: 2 2\n";
    }
    return Move{};
}

Move randomPlayer(const Game& game, const GameState& state) {
    // O relógio entra na semente porque em alguns compiladores (MinGW antigo)
    // o random_device devolve sempre a mesma sequência.
    static std::seed_seq seed{
        std::random_device{}(),
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
    static std::mt19937 random{seed};

    const std::vector<Move> moves = game.actions(state);
    if (moves.empty()) {
        return Move{};
    }
    std::uniform_int_distribution<std::size_t> pick(0, moves.size() - 1);
    return moves[pick(random)];
}

Move alphabetaPlayer(const Game& game, const GameState& state) {
    return alphabetaSearch(state, game);
}

Move mctsPlayer(const Game& game, const GameState& state) {
    return monteCarloTreeSearch(state, game);
}
