cmake_minimum_required(VERSION 3.16)
project(game_ai_lab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tictactoe
    src/main.cpp
    src/Game.cpp
    src/TicTacToe.cpp
    src/Minimax.cpp
    src/MonteCarloTreeSearch.cpp
    src/Players.cpp
)

target_include_directories(tictactoe PRIVATE include)
target_compile_options(tictactoe PRIVATE -Wall -Wextra -Wpedantic)
