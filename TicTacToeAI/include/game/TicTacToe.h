#pragma once

#include "game/Game.h"

// Jogo da velha em um tabuleiro h x v, vence quem fizer k em linha. Max é o
// primeiro jogador e joga com X. O tabuleiro guarda só as casas ocupadas e
// moves guarda as casas livres.
class TicTacToe : public Game {
public:
    explicit TicTacToe(int h = 3, int v = 3, int k = 3);

    // Jogadas permitidas são as casas ainda livres.
    std::vector<Move> actions(const GameState& state) const override;

    // Jogada inválida não tem efeito.
    GameState result(const GameState& state, const Move& move) const override;

    // 1 para vitória, -1 para derrota, 0 caso contrário.
    int utility(const GameState& state, char player) const override;

    // Um estado é final se alguém venceu ou se não há casas livres.
    bool terminalTest(const GameState& state) const override;

    void display(const GameState& state) const override;

private:
    // 1 se X vence com esta jogada, -1 se O vence, 0 caso contrário.
    int computeUtility(const std::map<Move, char>& board, const Move& move, char player) const;

    // Verdadeiro se há uma linha de k peças de player passando por move.
    bool kInRow(const std::map<Move, char>& board, const Move& move, char player,
                const Move& delta) const;

    int h_;
    int v_;
    int k_;
};
