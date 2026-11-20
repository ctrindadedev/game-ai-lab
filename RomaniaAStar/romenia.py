"""Grafo da Romênia do exercício da aula 26 (slides 4.1, último slide).

Os dados foram transcritos do slide: as estradas com suas distâncias e a
tabela de distâncias em linha reta até Bucharest, que é a heurística h.

Rodar este arquivo confere a transcrição (contagem de cidades e estradas,
h(Bucharest) = 0 e h admissível nas estradas que chegam a Bucharest).
"""

INICIO = "Arad"
OBJETIVO = "Bucharest"

# Estradas do mapa (não direcionadas), em km.
ESTRADAS = [
    ("Arad", "Zerind", 75),
    ("Arad", "Sibiu", 140),
    ("Arad", "Timisoara", 118),
    ("Zerind", "Oradea", 71),
    ("Oradea", "Sibiu", 151),
    ("Timisoara", "Lugoj", 111),
    ("Lugoj", "Mehadia", 70),
    ("Mehadia", "Drobeta", 75),
    ("Drobeta", "Craiova", 120),
    ("Craiova", "Rimnicu Vilcea", 146),
    ("Craiova", "Pitesti", 138),
    ("Sibiu", "Fagaras", 99),
    ("Sibiu", "Rimnicu Vilcea", 80),
    ("Rimnicu Vilcea", "Pitesti", 97),
    ("Fagaras", "Bucharest", 211),
    ("Pitesti", "Bucharest", 101),
    ("Bucharest", "Giurgiu", 90),
    ("Bucharest", "Urziceni", 85),
    ("Urziceni", "Hirsova", 98),
    ("Hirsova", "Eforie", 86),
    ("Urziceni", "Vaslui", 142),
    ("Vaslui", "Iasi", 92),
    ("Iasi", "Neamt", 87),
]

# h(n): distância em linha reta de n até Bucharest (tabela do slide).
H_LINHA_RETA = {
    "Arad": 366,
    "Bucharest": 0,
    "Craiova": 160,
    "Drobeta": 242,
    "Eforie": 161,
    "Fagaras": 176,
    "Giurgiu": 77,
    "Hirsova": 151,
    "Iasi": 226,
    "Lugoj": 244,
    "Mehadia": 241,
    "Neamt": 234,
    "Oradea": 380,
    "Pitesti": 100,
    "Rimnicu Vilcea": 193,
    "Sibiu": 253,
    "Timisoara": 329,
    "Urziceni": 80,
    "Vaslui": 199,
    "Zerind": 374,
}


def vizinhos() -> dict[str, dict[str, int]]:
    """Lista de adjacência: vizinhos[cidade][vizinha] = distância."""
    adj: dict[str, dict[str, int]] = {c: {} for c in H_LINHA_RETA}
    for a, b, d in ESTRADAS:
        adj[a][b] = d
        adj[b][a] = d
    return adj


def conferir() -> None:
    adj = vizinhos()
    cidades_nas_estradas = {c for a, b, _ in ESTRADAS for c in (a, b)}
    assert cidades_nas_estradas == set(H_LINHA_RETA), "cidade sem h ou h sem cidade"
    assert len(H_LINHA_RETA) == 20 and len(ESTRADAS) == 23
    assert H_LINHA_RETA[OBJETIVO] == 0
    # h nunca passa do custo real de uma estrada direta até o objetivo
    for cidade, d in adj[OBJETIVO].items():
        assert H_LINHA_RETA[cidade] <= d, f"h({cidade}) superestima"
    print(f"ok: {len(H_LINHA_RETA)} cidades, {len(ESTRADAS)} estradas, {INICIO} -> {OBJETIVO}")


if __name__ == "__main__":
    conferir()
