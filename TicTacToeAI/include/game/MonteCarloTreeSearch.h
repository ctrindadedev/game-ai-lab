#pragma once

#include "game/Game.h"

// Melhor jogada com N iterações de Monte Carlo Tree Search: seleciona uma folha
// pelo UCB, expande, simula uma partida aleatória e retropropaga o resultado.
// Devolve a jogada do filho da raiz mais visitado.
Move monteCarloTreeSearch(const GameState& state, const Game& game, int N = 1000);
