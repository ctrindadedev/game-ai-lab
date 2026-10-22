#include "game/Game.h"

int Game::playGame(const std::vector<Player>& players) const {
    GameState state = initial_;
    while (true) {
        for (const Player& player : players) {
            const Move move = player(*this, state);
            state = result(state, move);
            if (terminalTest(state)) {
                display(state);
                return utility(state, toMove(initial_));
            }
        }
    }
}
