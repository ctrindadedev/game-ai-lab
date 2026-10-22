#pragma once

#include <functional>

#include "game/Game.h"

using CutoffTest = std::function<bool(const GameState& state, int depth)>;
using EvalFn = std::function<double(const GameState& state)>;

// Melhor jogada buscando até os estados finais. [Figura 5.3]
Move minimaxDecision(const GameState& state, const Game& game);

// Melhor jogada com poda alfa-beta, buscando até as folhas. [Figura 5.7]
Move alphabetaSearch(const GameState& state, const Game& game);

// Melhor jogada com poda alfa-beta, cortando a busca e usando uma função de
// avaliação. Sem cutoffTest, corta na profundidade d ou em estado final. Sem
// evalFn, usa a utilidade do estado para o jogador da vez.
Move alphabetaCutoffSearch(const GameState& state, const Game& game, int d = 4,
                           CutoffTest cutoffTest = nullptr, EvalFn evalFn = nullptr);
