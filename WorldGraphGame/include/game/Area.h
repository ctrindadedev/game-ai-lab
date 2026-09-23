#pragma once

#include <vector>

#include "game/Item.h"
#include "game/NonPlayerCharacter.h"
#include "game/Rectangle.h"

class Player;

enum class AreaState {
    INACTIVE,
    ACTIVE
};

// Um nó do grafo do mundo: uma região retangular com seus inimigos e itens.
class Area {
public:
    Area() = default;
    Area(int identifier, const Rectangle& bounds);

    int identifier() const { return identifier_; }
    const Rectangle& bounds() const { return bounds_; }

    AreaState state() const { return state_; }
    void setState(AreaState state) { state_ = state; }
    bool isActive() const { return state_ == AreaState::ACTIVE; }

    std::vector<NonPlayerCharacter>& characters() { return characters_; }
    const std::vector<NonPlayerCharacter>& characters() const { return characters_; }

    std::vector<Item>& items() { return items_; }
    const std::vector<Item>& items() const { return items_; }

    // Atualiza todos os inimigos vivos da área; chamado apenas para áreas ativas.
    void updateCharacters(float deltaTime, Player& player);

    // Testa a colisão do jogador com os itens e aplica os efeitos deles.
    void collectItems(Player& player);

    // Remove os inimigos mortos e os itens já coletados.
    void removeDeadEntities();

private:
    int identifier_ = -1;
    Rectangle bounds_{};
    AreaState state_ = AreaState::INACTIVE;
    std::vector<NonPlayerCharacter> characters_;
    std::vector<Item> items_;
};
