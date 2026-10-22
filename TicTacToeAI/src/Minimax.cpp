#include "game/Minimax.h"

#include <algorithm>
#include <limits>

namespace {

constexpr double infinity = std::numeric_limits<double>::infinity();

// minimax_decision

double minimaxMinValue(const GameState& state, const Game& game, char player);

double minimaxMaxValue(const GameState& state, const Game& game, char player) {
    if (game.terminalTest(state)) {
        return game.utility(state, player);
    }
    double v = -infinity;
    for (const Move& a : game.actions(state)) {
        v = std::max(v, minimaxMinValue(game.result(state, a), game, player));
    }
    return v;
}

double minimaxMinValue(const GameState& state, const Game& game, char player) {
    if (game.terminalTest(state)) {
        return game.utility(state, player);
    }
    double v = infinity;
    for (const Move& a : game.actions(state)) {
        v = std::min(v, minimaxMaxValue(game.result(state, a), game, player));
    }
    return v;
}

// alphabeta_search

double alphabetaMinValue(const GameState& state, const Game& game, char player, double alpha,
                         double beta);

double alphabetaMaxValue(const GameState& state, const Game& game, char player, double alpha,
                         double beta) {
    if (game.terminalTest(state)) {
        return game.utility(state, player);
    }
    double v = -infinity;
    for (const Move& a : game.actions(state)) {
        v = std::max(v, alphabetaMinValue(game.result(state, a), game, player, alpha, beta));
        if (v >= beta) {
            return v;
        }
        alpha = std::max(alpha, v);
    }
    return v;
}

double alphabetaMinValue(const GameState& state, const Game& game, char player, double alpha,
                         double beta) {
    if (game.terminalTest(state)) {
        return game.utility(state, player);
    }
    double v = infinity;
    for (const Move& a : game.actions(state)) {
        v = std::min(v, alphabetaMaxValue(game.result(state, a), game, player, alpha, beta));
        if (v <= alpha) {
            return v;
        }
        beta = std::min(beta, v);
    }
    return v;
}

// alphabeta_cutoff_search

double cutoffMinValue(const GameState& state, const Game& game, const CutoffTest& cutoffTest,
                      const EvalFn& evalFn, double alpha, double beta, int depth);

double cutoffMaxValue(const GameState& state, const Game& game, const CutoffTest& cutoffTest,
                      const EvalFn& evalFn, double alpha, double beta, int depth) {
    if (cutoffTest(state, depth)) {
        return evalFn(state);
    }
    double v = -infinity;
    for (const Move& a : game.actions(state)) {
        v = std::max(v, cutoffMinValue(game.result(state, a), game, cutoffTest, evalFn, alpha,
                                       beta, depth + 1));
        if (v >= beta) {
            return v;
        }
        alpha = std::max(alpha, v);
    }
    return v;
}

double cutoffMinValue(const GameState& state, const Game& game, const CutoffTest& cutoffTest,
                      const EvalFn& evalFn, double alpha, double beta, int depth) {
    if (cutoffTest(state, depth)) {
        return evalFn(state);
    }
    double v = infinity;
    for (const Move& a : game.actions(state)) {
        v = std::min(v, cutoffMaxValue(game.result(state, a), game, cutoffTest, evalFn, alpha,
                                       beta, depth + 1));
        if (v <= alpha) {
            return v;
        }
        beta = std::min(beta, v);
    }
    return v;
}

}  // namespace

Move minimaxDecision(const GameState& state, const Game& game) {
    const char player = game.toMove(state);

    // argmax: em caso de empate, fica a primeira jogada com o maior valor.
    Move bestAction{};
    double bestValue = -infinity;
    for (const Move& a : game.actions(state)) {
        const double value = minimaxMinValue(game.result(state, a), game, player);
        if (value > bestValue) {
            bestValue = value;
            bestAction = a;
        }
    }
    return bestAction;
}

Move alphabetaSearch(const GameState& state, const Game& game) {
    const char player = game.toMove(state);

    double bestScore = -infinity;
    const double beta = infinity;
    Move bestAction{};
    for (const Move& a : game.actions(state)) {
        const double v = alphabetaMinValue(game.result(state, a), game, player, bestScore, beta);
        if (v > bestScore) {
            bestScore = v;
            bestAction = a;
        }
    }
    return bestAction;
}

Move alphabetaCutoffSearch(const GameState& state, const Game& game, int d,
                           CutoffTest cutoffTest, EvalFn evalFn) {
    const char player = game.toMove(state);

    // O teste padrão corta na profundidade d ou em estado final.
    if (!cutoffTest) {
        cutoffTest = [&game, d](const GameState& s, int depth) {
            return depth > d || game.terminalTest(s);
        };
    }
    if (!evalFn) {
        evalFn = [&game, player](const GameState& s) { return game.utility(s, player); };
    }

    double bestScore = -infinity;
    const double beta = infinity;
    Move bestAction{};
    for (const Move& a : game.actions(state)) {
        const double v =
            cutoffMinValue(game.result(state, a), game, cutoffTest, evalFn, bestScore, beta, 1);
        if (v > bestScore) {
            bestScore = v;
            bestAction = a;
        }
    }
    return bestAction;
}
