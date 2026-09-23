#pragma once

#include <string>
#include <vector>

class Viewport;
class WorldGraph;
class Player;

class Renderer {
public:
    Renderer() = default;
    Renderer(int columns, int rows);

    // Monta o quadro (jogador, inimigos, itens) num buffer interno; não
    // imprime nada ainda.
    void draw(const WorldGraph& world, const Viewport& viewport, const Player& player);

    // Acrescenta a barra de status como última linha do buffer e imprime o
    // quadro inteiro de uma vez, reposicionando o cursor no início.
    void drawStatusBar(const Player& player, float remainingTime,
                       const std::vector<int>& activeAreas);

private:
    int columns_ = 80;
    int rows_ = 24;
    std::string frameBuffer_;
};
