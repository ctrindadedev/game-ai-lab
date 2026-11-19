"""Geradores de mapas e utilitários para os experimentos de cenário.

Todos os mapas seguem o formato do Gymnasium (lista de strings com S, F, H, G),
então podem ser passados direto para `a_star` e `executar_no_ambiente`.
"""

from __future__ import annotations

import math
import random
from collections import deque
from typing import Iterable, Optional

from astar import (Celula, a_star, encontrar, executar_no_ambiente, heuristica_nula,
                   ler_mapa, manhattan, validar_caminho, vizinhos)


# ---------------------------------------------------------------------------
# Montagem de mapas
# ---------------------------------------------------------------------------

def montar_mapa(tamanho: int, inicio: Celula = (0, 0), objetivo: Optional[Celula] = None,
                p_livre: float = 1.0, buracos: Iterable[Celula] = (),
                semente: Optional[int] = None) -> list[str]:
    """Mapa quadrado com S e G onde se quiser.

    Cada célula vira buraco com probabilidade `1 - p_livre` (mesma convenção do
    `p` de `generate_random_map`). `buracos` acrescenta buracos em posições
    fixas. As células de S e G nunca são buraco. Não garante que exista caminho:
    use `existe_caminho` ou `mapa_com_caminho`.
    """
    objetivo = (tamanho - 1, tamanho - 1) if objetivo is None else objetivo
    rng = random.Random(semente)
    grid = [["H" if rng.random() >= p_livre else "F" for _ in range(tamanho)]
            for _ in range(tamanho)]
    for i, j in buracos:
        grid[i][j] = "H"
    grid[inicio[0]][inicio[1]] = "S"
    grid[objetivo[0]][objetivo[1]] = "G"
    return ["".join(l) for l in grid]


def posicionar(mapa: list[str], inicio: Celula, objetivo: Celula) -> list[str]:
    """Mesmo mapa (mesmos buracos) com S e G movidos para novas posições.

    As posições antigas de S e G viram gelo, e as novas deixam de ser buraco.
    """
    grid = [list(l.replace("S", "F").replace("G", "F")) for l in ler_mapa(mapa)]
    grid[inicio[0]][inicio[1]] = "S"
    grid[objetivo[0]][objetivo[1]] = "G"
    return ["".join(l) for l in grid]


def existe_caminho(mapa: list[str], inicio: Optional[Celula] = None,
                   objetivo: Optional[Celula] = None) -> bool:
    """Busca em largura simples, independente do A*."""
    mapa = ler_mapa(mapa)
    inicio = encontrar(mapa, "S") if inicio is None else inicio
    objetivo = encontrar(mapa, "G") if objetivo is None else objetivo
    vistos, fila = {inicio}, deque([inicio])
    while fila:
        atual = fila.popleft()
        if atual == objetivo:
            return True
        for _, prox in vizinhos(mapa, atual):
            if prox not in vistos:
                vistos.add(prox)
                fila.append(prox)
    return False


def alcancaveis(mapa: list[str], inicio: Optional[Celula] = None) -> int:
    """Quantidade de células alcançáveis a partir do início (incluindo ele)."""
    mapa = ler_mapa(mapa)
    inicio = encontrar(mapa, "S") if inicio is None else inicio
    vistos, fila = {inicio}, deque([inicio])
    while fila:
        for _, prox in vizinhos(mapa, fila.popleft()):
            if prox not in vistos:
                vistos.add(prox)
                fila.append(prox)
    return len(vistos)


def mapa_com_caminho(tamanho: int, inicio: Celula = (0, 0), objetivo: Optional[Celula] = None,
                     p_livre: float = 0.8, semente: int = 0, max_tentativas: int = 1000):
    """Sorteia mapas até um ter caminho de S a G. Devolve (mapa, tentativas)."""
    for k in range(max_tentativas):
        mapa = montar_mapa(tamanho, inicio, objetivo, p_livre, semente=semente * 10_000 + k)
        if existe_caminho(mapa):
            return mapa, k + 1
    raise RuntimeError("nenhum mapa com caminho; aumente p_livre ou max_tentativas")


# ---------------------------------------------------------------------------
# Padrões montados à mão
# ---------------------------------------------------------------------------

def mapa_parede(tamanho: int, linha_passagem: int) -> list[str]:
    """Parede vertical no meio do mapa com uma única passagem.

    S fica no meio da coluna da esquerda e G no meio da coluna da direita.
    Com a passagem alinhada a S e G a Manhattan acerta; com ela num extremo,
    o caminho real fica bem mais longo que a estimativa.
    """
    meio = tamanho // 2
    buracos = [(i, meio) for i in range(tamanho) if i != linha_passagem]
    return montar_mapa(tamanho, (meio, 0), (meio, tamanho - 1), buracos=buracos)


def mapa_armadilha_u(tamanho: int) -> list[str]:
    """Armadilha em "U" aberta para o S, entre S e G.

    A Manhattan puxa a busca para dentro do U (que fica na direção do G) e o
    A* precisa esgotar o interior antes de contornar a armadilha.
    """
    meio = tamanho // 2
    topo, base = meio - tamanho // 4, meio + tamanho // 4
    fundo = tamanho - tamanho // 4            # coluna do fundo do U
    boca = tamanho // 4                       # coluna onde os braços começam
    buracos = [(i, fundo) for i in range(topo, base + 1)]
    buracos += [(topo, j) for j in range(boca, fundo)]
    buracos += [(base, j) for j in range(boca, fundo)]
    return montar_mapa(tamanho, (meio, 1), (meio, tamanho - 1), buracos=buracos)


def mapa_labirinto(tamanho: int, semente: int = 0, abrir: float = 0.0) -> list[str]:
    """Labirinto por backtracking recursivo, com S em (0,0) e G no canto oposto.

    `tamanho` deve ser ímpar. Com `abrir` > 0, essa fração das paredes internas
    é removida, criando ciclos (mais de um caminho possível).
    """
    if tamanho % 2 == 0:
        raise ValueError("o labirinto precisa de tamanho ímpar")
    rng = random.Random(semente)
    grid = [["H"] * tamanho for _ in range(tamanho)]
    grid[0][0] = "F"
    pilha = [(0, 0)]
    while pilha:
        i, j = pilha[-1]
        opcoes = [(i + di, j + dj, i + di // 2, j + dj // 2)
                  for di, dj in ((0, 2), (2, 0), (0, -2), (-2, 0))
                  if 0 <= i + di < tamanho and 0 <= j + dj < tamanho
                  and grid[i + di][j + dj] == "H"]
        if not opcoes:
            pilha.pop()
            continue
        ni, nj, pi, pj = rng.choice(opcoes)
        grid[pi][pj] = grid[ni][nj] = "F"
        pilha.append((ni, nj))
    if abrir > 0:
        paredes = [(i, j) for i in range(tamanho) for j in range(tamanho)
                   if grid[i][j] == "H" and (i % 2) != (j % 2)]
        for i, j in rng.sample(paredes, int(abrir * len(paredes))):
            grid[i][j] = "F"
    grid[0][0], grid[-1][-1] = "S", "G"
    return ["".join(l) for l in grid]


def mapa_objetivo_isolado(tamanho: int) -> list[str]:
    """G cercado por um anel de buracos: não existe caminho."""
    g = (tamanho - 3, tamanho - 3)
    anel = [(g[0] + di, g[1] + dj) for di in (-1, 0, 1) for dj in (-1, 0, 1)
            if (di, dj) != (0, 0)]
    return montar_mapa(tamanho, (0, 0), g, buracos=anel)


# ---------------------------------------------------------------------------
# Heurísticas extras e execução dos experimentos
# ---------------------------------------------------------------------------

def manhattan_ponderada(w: float):
    """h = w · Manhattan. Para w > 1 superestima, então deixa de ser admissível."""
    def h(celula: Celula, objetivo: Celula) -> float:
        return w * manhattan(celula, objetivo)
    h.__name__ = f"manhattan_x{w:g}"
    return h


def rodar(mapa: list[str], heuristicas: dict, inicio: Optional[Celula] = None,
          objetivo: Optional[Celula] = None, conferir_no_gym: bool = True) -> list[dict]:
    """Roda o A* com cada heurística e devolve uma linha de métricas por execução.

    Cada caminho é validado. Quando a busca vai de S a G, ele também é executado
    no Gymnasium (precisa terminar com recompensa 1). `otimo` é o custo de
    Dijkstra (h = 0), usado para medir quanto uma heurística não admissível
    pode errar.
    """
    otimo = a_star(mapa, inicio, objetivo, heuristica_fn=heuristica_nula)
    linhas = []
    for nome, h in heuristicas.items():
        r = a_star(mapa, inicio, objetivo, heuristica_fn=h)
        validar_caminho(mapa, r)
        if conferir_no_gym and r.encontrou and inicio is None and objetivo is None:
            recompensa, terminou, _ = executar_no_ambiente(mapa, r.acoes)
            assert terminou and recompensa == 1.0, "o caminho não chegou ao G no Gymnasium"
        excesso = (r.custo / otimo.custo - 1) * 100 if otimo.encontrou else math.nan
        linhas.append({"heuristica": nome, **r.metricas(), "otimo": otimo.custo,
                       "excesso_%": excesso})
    return linhas
