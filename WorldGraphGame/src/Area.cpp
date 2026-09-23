#include "game/Area.h"

#include <algorithm>

#include "game/Player.h"

Area::Area(int identifier, const Rectangle& bounds)
    : identifier_(identifier), bounds_(bounds) {}

void Area::updateCharacters(float deltaTime, Player& player) {
    for (NonPlayerCharacter& character : characters_) {
        if (!character.isDead()) {
            character.update(deltaTime, player);
        }
    }
}

void Area::collectItems(Player& player, float ammunitionItemRadius) {
    for (Item& item : items_) {
        if (item.collected) {
            continue;
        }
        if (distance(item.position, player.position()) > (item.radius + player.radius())) {
            continue;
        }

        item.collected = true;
        if (item.type == ItemType::HEALTH) {
            player.heal(item.value);
        } else {
            for (NonPlayerCharacter& character : characters_) {
                if (!character.isDead() &&
                    distance(character.position(), item.position) <= ammunitionItemRadius) {
                    character.takeDamage(item.value);
                }
            }
        }
    }
}

void Area::removeDeadEntities() {
    characters_.erase(
        std::remove_if(characters_.begin(), characters_.end(),
                       [](const NonPlayerCharacter& character) { return character.isDead(); }),
        characters_.end());

    items_.erase(std::remove_if(items_.begin(), items_.end(),
                                [](const Item& item) { return item.collected; }),
                items_.end());
}
