#include "game/MonteCarloTreeSearch.h"

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <utility>
#include <vector>

namespace {

constexpr double infinity = std::numeric_limits<double>::infinity();

// MCT_Node (utils.py): nó da árvore de busca. children liga cada filho à
// jogada que leva até ele, na ordem de game.actions, como o dict do original.
struct MctNode {
    MctNode* parent = nullptr;
    GameState state;
    double U = 0;  // vitórias
    double N = 0;  // visitas
    std::vector<std::pair<std::unique_ptr<MctNode>, Move>> children;
};

// ucb (utils.py): infinito para nós não visitados.
double ucb(const MctNode& n, double C = 1.4) {
    if (n.N == 0) {
        return infinity;
    }
    return n.U / n.N + C * std::sqrt(std::log(n.parent->N) / n.N);
}

// Desce pelo filho de maior ucb até uma folha. Em caso de empate, fica o
// primeiro, como o max do Python.
MctNode* select(MctNode* n) {
    if (n->children.empty()) {
        return n;
    }
    MctNode* best = nullptr;
    double bestUcb = -infinity;
    for (const auto& [child, move] : n->children) {
        const double value = ucb(*child);
        if (best == nullptr || value > bestUcb) {
            bestUcb = value;
            best = child.get();
        }
    }
    return select(best);
}

// Cria um filho para cada jogada da folha, se ela não for final.
MctNode* expand(MctNode* n, const Game& game) {
    if (n->children.empty() && !game.terminalTest(n->state)) {
        for (const Move& action : game.actions(n->state)) {
            auto child = std::make_unique<MctNode>();
            child->parent = n;
            child->state = game.result(n->state, action);
            n->children.emplace_back(std::move(child), action);
        }
    }
    return select(n);
}

// Joga ao acaso até o fim e devolve a utilidade com o sinal trocado para o
// jogador da vez no estado de partida.
double simulate(const Game& game, GameState state) {
    // Mesma semente do randomPlayer: no MinGW antigo o random_device devolve
    // sempre a mesma sequência.
    static std::seed_seq seed{
        std::random_device{}(),
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
    static std::mt19937 random{seed};

    const char player = game.toMove(state);
    while (!game.terminalTest(state)) {
        const std::vector<Move> actions = game.actions(state);
        std::uniform_int_distribution<std::size_t> pick(0, actions.size() - 1);
        state = game.result(state, actions[pick(random)]);
    }
    const double v = game.utility(state, player);
    return -v;
}

// Passa a utilidade para os ancestrais, trocando o sinal a cada nível.
void backprop(MctNode* n, double utility) {
    if (utility > 0) {
        n->U += utility;
    }
    // No original, o empate valendo 0,5 está comentado:
    // if (utility == 0) { n->U += 0.5; }
    n->N += 1;
    if (n->parent != nullptr) {
        backprop(n->parent, -utility);
    }
}

}  // namespace

Move monteCarloTreeSearch(const GameState& state, const Game& game, int N) {
    MctNode root;
    root.state = state;

    for (int i = 0; i < N; ++i) {
        MctNode* leaf = select(&root);
        MctNode* child = expand(leaf, game);
        const double result = simulate(game, child->state);
        backprop(child, result);
    }

    // Filho mais visitado. Em caso de empate, fica o primeiro.
    Move bestAction{};
    double bestN = -1;
    for (const auto& [child, move] : root.children) {
        if (child->N > bestN) {
            bestN = child->N;
            bestAction = move;
        }
    }
    return bestAction;
}
