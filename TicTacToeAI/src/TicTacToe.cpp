#include "game/TicTacToe.h"

#include <algorithm>
#include <iostream>

TicTacToe::TicTacToe(int h, int v, int k) : h_(h), v_(v), k_(k) {
    std::vector<Move> moves;
    for (int x = 1; x <= h; ++x) {
        for (int y = 1; y <= v; ++y) {
            moves.emplace_back(x, y);
        }
    }
    initial_ = GameState{'X', 0, {}, moves};
}

std::vector<Move> TicTacToe::actions(const GameState& state) const {
    return state.moves;
}

GameState TicTacToe::result(const GameState& state, const Move& move) const {
    if (std::find(state.moves.begin(), state.moves.end(), move) == state.moves.end()) {
        return state;
    }
    std::map<Move, char> board = state.board;
    board[move] = state.toMove;
    std::vector<Move> moves = state.moves;
    moves.erase(std::find(moves.begin(), moves.end(), move));
    return GameState{state.toMove == 'X' ? 'O' : 'X',
                     computeUtility(board, move, state.toMove), board, moves};
}

int TicTacToe::utility(const GameState& state, char player) const {
    return player == 'X' ? state.utility : -state.utility;
}

bool TicTacToe::terminalTest(const GameState& state) const {
    return state.utility != 0 || state.moves.empty();
}

void TicTacToe::display(const GameState& state) const {
    for (int x = 1; x <= h_; ++x) {
        for (int y = 1; y <= v_; ++y) {
            const auto cell = state.board.find({x, y});
            std::cout << (cell != state.board.end() ? cell->second : '.') << ' ';
        }
        std::cout << '\n';
    }
}

int TicTacToe::computeUtility(const std::map<Move, char>& board, const Move& move,
                              char player) const {
    if (kInRow(board, move, player, {0, 1}) || kInRow(board, move, player, {1, 0}) ||
        kInRow(board, move, player, {1, -1}) || kInRow(board, move, player, {1, 1})) {
        return player == 'X' ? +1 : -1;
    }
    return 0;
}

bool TicTacToe::kInRow(const std::map<Move, char>& board, const Move& move, char player,
                       const Move& delta) const {
    const auto owns = [&](int x, int y) {
        const auto cell = board.find({x, y});
        return cell != board.end() && cell->second == player;
    };

    int n = 0;  // número de peças em linha
    int x = move.first;
    int y = move.second;
    while (owns(x, y)) {
        ++n;
        x += delta.first;
        y += delta.second;
    }
    x = move.first;
    y = move.second;
    while (owns(x, y)) {
        ++n;
        x -= delta.first;
        y -= delta.second;
    }
    --n;  // a própria jogada foi contada duas vezes
    return n >= k_;
}
