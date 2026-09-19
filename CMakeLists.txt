cmake_minimum_required(VERSION 3.16)
project(game_ai_lab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(game
    src/main.cpp
    src/Character.cpp
    src/Player.cpp
    src/NonPlayerCharacter.cpp
    src/Area.cpp
    src/WorldGraph.cpp
    src/Viewport.cpp
    src/LevelConfiguration.cpp
    src/Renderer.cpp
    src/Game.cpp
)

target_include_directories(game PRIVATE include)
target_compile_options(game PRIVATE -Wall -Wextra -Wpedantic)
