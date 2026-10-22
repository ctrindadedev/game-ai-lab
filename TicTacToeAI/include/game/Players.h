#pragma once

#include "game/Game.h"

// Jogadores do aima-python. Todos seguem a assinatura de Player.

// Lê a jogada da entrada padrão.
Move queryPlayer(const Game& game, const GameState& state);

// Escolhe uma jogada válida ao acaso.
Move randomPlayer(const Game& game, const GameState& state);

// Minimax com poda alfa-beta até as folhas.
Move alphabetaPlayer(const Game& game, const GameState& state);
