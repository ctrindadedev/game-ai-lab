#pragma once

#include <functional>
#include <map>
#include <utility>
#include <vector>

// Tradução para C++ do framework de jogos do aima-python (games4e.py),
// capítulo 5 de Artificial Intelligence: A Modern Approach.

// Posição (x, y) no tabuleiro, começando em 1.
using Move = std::pair<int, int>;

struct GameState {
    char toMove = 'X';
    int utility = 0;  // utilidade em cache do estado
    std::map<Move, char> board;
    std::vector<Move> moves;
};

class Game;

// Um jogador é uma função que recebe o jogo e o estado e devolve uma jogada.
using Player = std::function<Move(const Game&, const GameState&)>;

// Para criar um jogo, herde desta classe, implemente actions, result, utility
// e display e preencha initial_ no construtor.
class Game {
public:
    virtual ~Game() = default;

    // Jogadas permitidas neste estado.
    virtual std::vector<Move> actions(const GameState& state) const = 0;

    // Estado que resulta de fazer a jogada move no estado state.
    virtual GameState result(const GameState& state, const Move& move) const = 0;

    // Valor deste estado final para player.
    virtual int utility(const GameState& state, char player) const = 0;

    // Verdadeiro se o estado é final.
    virtual bool terminalTest(const GameState& state) const { return actions(state).empty(); }

    // Jogador da vez neste estado.
    virtual char toMove(const GameState& state) const { return state.toMove; }

    virtual void display(const GameState& state) const = 0;

    // Partida com jogadas alternadas entre os jogadores. Devolve a utilidade
    // do estado final para quem começou.
    int playGame(const std::vector<Player>& players) const;

    const GameState& initial() const { return initial_; }

protected:
    GameState initial_;
};
