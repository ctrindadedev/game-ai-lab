#include "game/Game.h"

Game::Game(const LevelConfiguration& configuration)
    : configuration_(configuration),
      player_({0.0f, 0.0f}, configuration.playerHealth),
      viewport_({0.0f, 0.0f}, {configuration.viewportWidth, configuration.viewportHeight}),
      renderer_(),
      remainingTime_(configuration.survivalTime) {
    player_.setSpeed(configuration.playerSpeed);
    world_.build(configuration_);
}

void Game::run() {
    update(0.0f);
    render();
}

void Game::processInput() {
}

void Game::update(float deltaTime) {
    if (state_ != GameState::RUNNING) {
        return;
    }

    processInput();
    player_.update(deltaTime);

    world_.updateActiveAreas(player_.position(), configuration_);
    for (int areaIdentifier : world_.activeAreas()) {
        if (Area* area = world_.area(areaIdentifier)) {
            area->updateCharacters(deltaTime, player_);
            area->collectItems(player_);
            area->removeDeadEntities();
        }
    }

    viewport_.follow(player_.position(), deltaTime, configuration_.viewportSmoothing);

    remainingTime_ -= deltaTime;
    if (player_.isDead()) {
        state_ = GameState::DEFEAT;
    } else if (remainingTime_ <= 0.0f) {
        state_ = GameState::VICTORY;
    }
}

void Game::render() {
    renderer_.draw(world_, viewport_, player_);
    renderer_.drawStatusBar(player_, remainingTime_, world_.activeAreas().size());
}
