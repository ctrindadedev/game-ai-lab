#include "game/Game.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <iostream>
#include <optional>
#include <thread>

#include "game/Terminal.h"

namespace {

constexpr float fixedStep = 1.0f / 60.0f;
constexpr float maximumFrameTime = 0.25f;
constexpr std::chrono::microseconds frameDuration{1'000'000 / 60};

volatile std::sig_atomic_t interruptRequested = 0;

void handleInterrupt(int) {
    interruptRequested = 1;
}

}

Game::Game(const LevelConfiguration& configuration)
    : configuration_(configuration),
      player_({0.0f, 0.0f}, configuration.playerHealth),
      viewport_({0.0f, 0.0f}, {configuration.viewportWidth, configuration.viewportHeight}),
      renderer_(),
      remainingTime_(configuration.survivalTime) {
    player_.setSpeed(configuration.playerSpeed);
    world_.build(configuration_);

    const Vector2 start = world_.bounds().center();
    player_.setPosition(start);
    viewport_.setCenter(start);
    world_.updateActiveAreas(start, configuration_, 0.0f);
}

void Game::run() {
    {
        RawTerminal terminal;
        interruptRequested = 0;
        const auto previousHandler = std::signal(SIGINT, handleInterrupt);

        using Clock = std::chrono::steady_clock;
        auto previousTime = Clock::now();
        float accumulator = 0.0f;

        while (state_ == GameState::RUNNING) {
            const auto frameStart = Clock::now();
            const float elapsed = std::chrono::duration<float>(frameStart - previousTime).count();
            previousTime = frameStart;
            accumulator += std::min(elapsed, maximumFrameTime);

            if (interruptRequested) {
                state_ = GameState::INTERRUPTED;
                break;
            }

            processInput(terminal.readAvailable());
            while (accumulator >= fixedStep && state_ == GameState::RUNNING) {
                update(fixedStep);
                accumulator -= fixedStep;
            }
            render();

            std::this_thread::sleep_until(frameStart + frameDuration);
        }

        std::signal(SIGINT, previousHandler);
    }

    std::cout << '\n';
    switch (state_) {
        case GameState::VICTORY:     std::cout << "Vitória: você sobreviveu!\n"; break;
        case GameState::DEFEAT:      std::cout << "Derrota: o jogador morreu.\n"; break;
        case GameState::INTERRUPTED: std::cout << "Jogo interrompido.\n"; break;
        case GameState::RUNNING:     break;
    }
}

void Game::processInput(const std::string& pressedKeys) {
    std::optional<Vector2> direction;
    for (std::size_t index = 0; index < pressedKeys.size(); ++index) {
        const char key = pressedKeys[index];

        if (key == '\033' && index + 2 < pressedKeys.size() && pressedKeys[index + 1] == '[') {
            switch (pressedKeys[index + 2]) {
                case 'A': direction = Vector2{0.0f, 1.0f};  break;
                case 'B': direction = Vector2{0.0f, -1.0f}; break;
                case 'C': direction = Vector2{1.0f, 0.0f};  break;
                case 'D': direction = Vector2{-1.0f, 0.0f}; break;
                default: break;
            }
            index += 2;
            continue;
        }

        switch (key) {
            case 'w': case 'W': direction = Vector2{0.0f, 1.0f};   break;
            case 's': case 'S': direction = Vector2{0.0f, -1.0f};  break;
            case 'd': case 'D': direction = Vector2{1.0f, 0.0f};   break;
            case 'a': case 'A': direction = Vector2{-1.0f, 0.0f};  break;
            case 'q': case 'Q': direction = Vector2{-1.0f, 1.0f};  break;
            case 'e': case 'E': direction = Vector2{1.0f, 1.0f};   break;
            case 'z': case 'Z': direction = Vector2{-1.0f, -1.0f}; break;
            case 'c': case 'C': direction = Vector2{1.0f, -1.0f};  break;
            case ' ':           direction = Vector2{0.0f, 0.0f};   break;
            default: break;
        }
    }

    if (direction) {
        player_.setInputDirection(*direction);
    }
}

void Game::update(float deltaTime) {
    if (state_ != GameState::RUNNING) {
        return;
    }

    player_.update(deltaTime);
    player_.setPosition(world_.bounds().clamp(player_.position()));

    world_.updateActiveAreas(player_.position(), configuration_, deltaTime);
    for (int areaIdentifier : world_.activeAreas()) {
        if (Area* area = world_.area(areaIdentifier)) {
            area->updateCharacters(deltaTime, player_);
            area->collectItems(player_, configuration_.ammunitionItemRadius);
            area->removeDeadEntities();
        }
    }

    viewport_.follow(player_.position(), deltaTime, configuration_.viewportSmoothing);

    remainingTime_ = std::max(remainingTime_ - deltaTime, 0.0f);
    if (player_.isDead()) {
        state_ = GameState::DEFEAT;
    } else if (remainingTime_ <= 0.0f) {
        state_ = GameState::VICTORY;
    }
}

void Game::render() {
    renderer_.draw(world_, viewport_, player_);
    renderer_.drawStatusBar(player_, remainingTime_, world_.activeAreas());
}
