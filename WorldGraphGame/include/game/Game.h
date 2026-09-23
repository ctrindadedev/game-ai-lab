#pragma once

#include <string>

#include "game/LevelConfiguration.h"
#include "game/Player.h"
#include "game/Renderer.h"
#include "game/Viewport.h"
#include "game/WorldGraph.h"

enum class GameState {
    RUNNING,
    VICTORY,
    DEFEAT,
    INTERRUPTED
};

class Game {
public:
    explicit Game(const LevelConfiguration& configuration);

    void run();

    void update(float deltaTime);
    void render();

    GameState state() const { return state_; }
    float remainingTime() const { return remainingTime_; }

private:
    void processInput(const std::string& pressedKeys);

    LevelConfiguration configuration_;
    WorldGraph world_;
    Player player_;
    Viewport viewport_;
    Renderer renderer_;
    GameState state_ = GameState::RUNNING;
    float remainingTime_ = 0.0f;
};
