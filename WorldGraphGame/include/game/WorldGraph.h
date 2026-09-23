#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "game/Area.h"
#include "game/Rectangle.h"
#include "game/Vector2.h"

struct LevelConfiguration;

// Grafo do mundo: cada nó é uma Area e as arestas são adjacências entre áreas.
// As áreas ficam em unique_ptr para permitir carga e descarte sob demanda.
class WorldGraph {
public:
    WorldGraph() = default;

    // Constrói a malha e as adjacências a partir da configuração do nível.
    void build(const LevelConfiguration& configuration);

    std::size_t areaCount() const { return nodes_.size(); }

    // Metadados leves: continuam válidos mesmo com a área descarregada.
    const Rectangle& boundsOf(int areaIdentifier) const;
    const std::vector<int>& neighbors(int areaIdentifier) const;

    // Ponteiro para a área carregada, ou nullptr se ela ainda não foi carregada.
    Area* area(int areaIdentifier);
    const Area* area(int areaIdentifier) const;

    // Carrega a área sob demanda, caso ela ainda não esteja em memória.
    Area& ensureLoaded(int areaIdentifier);
    // Libera a memória da área, preservando os metadados do nó.
    void unload(int areaIdentifier);

    // Identificador da área que contém a posição, se houver alguma.
    std::optional<int> areaAt(const Vector2& position) const;

    // Mantém a janela de vizinhança ativa: a área atual e os vizinhos próximos,
    // respeitando o limite máximo de áreas ativas.
    void updateActiveAreas(const Vector2& playerPosition,
                           const LevelConfiguration& configuration);

    const std::vector<int>& activeAreas() const { return activeAreas_; }

private:
    // Metadados permanentes de um nó do grafo.
    struct Node {
        int identifier = -1;
        Rectangle bounds{};
        std::vector<int> neighbors;
        std::unique_ptr<Area> area;
    };

    std::vector<Node> nodes_;
    std::vector<int> activeAreas_;
};
