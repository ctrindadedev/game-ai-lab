"""Implementação do A* no Lago Congelado.

O mapa é uma lista de strings no formato do Gymnasium ("S", "F", "H", "G"),
e uma célula é uma tupla (linha, coluna). Buracos ("H") são intransitáveis.

O A* recebe duas funções, para que os experimentos mudem o custo e a
heurística sem mexer na busca:

    custo_fn(origem, destino) -> float      custo do passo origem -> destino
    heuristica_fn(celula, objetivo) -> float estimativa de celula até o objetivo
"""

from __future__ import annotations

import heapq
import math
import time
from dataclasses import dataclass, field
from typing import Callable, Optional

Celula = tuple[int, int]
CustoFn = Callable[[Celula, Celula], float]
HeuristicaFn = Callable[[Celula, Celula], float]

# Mesma numeração de ações do FrozenLake-v1 no Gymnasium.
ESQUERDA, BAIXO, DIREITA, CIMA = 0, 1, 2, 3
DESLOCAMENTOS = {
    ESQUERDA: (0, -1),
    BAIXO: (1, 0),
    DIREITA: (0, 1),
    CIMA: (-1, 0),
}
NOMES_ACOES = {ESQUERDA: "esquerda", BAIXO: "baixo", DIREITA: "direita", CIMA: "cima"}


# ---------------------------------------------------------------------------
# Mapa
# ---------------------------------------------------------------------------

def ler_mapa(desc) -> list[str]:
    """Converte um mapa em lista de strings.

    Aceita `env.unwrapped.desc` (array de bytes), uma lista de strings ou o
    retorno de `generate_random_map`.
    """
    linhas = []
    for linha in desc:
        if isinstance(linha, str):
            linhas.append(linha)
        else:
            linhas.append("".join(c.decode() if isinstance(c, bytes) else str(c) for c in linha))
    if len({len(l) for l in linhas}) != 1:
        raise ValueError("todas as linhas do mapa precisam ter o mesmo tamanho")
    return linhas


def encontrar(grid: list[str], letra: str) -> Celula:
    """Posição da primeira célula com a letra pedida (ex.: "S" ou "G")."""
    for i, linha in enumerate(grid):
        j = linha.find(letra)
        if j >= 0:
            return (i, j)
    raise ValueError(f"o mapa não tem a célula {letra!r}")


def transitavel(grid: list[str], celula: Celula) -> bool:
    i, j = celula
    return 0 <= i < len(grid) and 0 <= j < len(grid[0]) and grid[i][j] != "H"


def vizinhos(grid: list[str], celula: Celula):
    """Gera (ação, célula vizinha) para as vizinhas que não são buraco."""
    i, j = celula
    for acao, (di, dj) in DESLOCAMENTOS.items():
        prox = (i + di, j + dj)
        if transitavel(grid, prox):
            yield acao, prox


# ---------------------------------------------------------------------------
# Custos e heurísticas da aula
# ---------------------------------------------------------------------------

def custo_unitario(origem: Celula, destino: Celula) -> float:
    """Custo da aula: 1 por passo."""
    return 1.0


def heuristica_nula(celula: Celula, objetivo: Celula) -> float:
    """h = 0: o A* vira Dijkstra (busca de custo uniforme)."""
    return 0.0


def manhattan(celula: Celula, objetivo: Celula) -> float:
    return float(abs(celula[0] - objetivo[0]) + abs(celula[1] - objetivo[1]))


def euclidiana(celula: Celula, objetivo: Celula) -> float:
    return math.hypot(celula[0] - objetivo[0], celula[1] - objetivo[1])


# ---------------------------------------------------------------------------
# A*
# ---------------------------------------------------------------------------

@dataclass
class Node:
    """Nó da árvore de busca (slide 35)."""

    estado: Celula
    pai: Optional["Node"]
    acao: Optional[int]  # ação que levou do pai a este nó (None na raiz)
    g: float             # custo do caminho desde a raiz


@dataclass
class Resultado:
    encontrou: bool
    caminho: list[Celula]          # do início ao objetivo (vazio se não houver)
    acoes: list[int]
    custo: float                   # g do objetivo (inf se não houver caminho)
    expandidos: int                # nós retirados da fronteira e expandidos
    gerados: int                   # nós colocados na fronteira
    max_fronteira: int
    tempo_ms: float
    fechados: list[Celula] = field(default_factory=list)  # ordem de expansão

    def metricas(self) -> dict:
        """Resumo para montar tabelas com pandas."""
        return {
            "encontrou": self.encontrou,
            "custo": self.custo,
            "passos": len(self.acoes),
            "expandidos": self.expandidos,
            "gerados": self.gerados,
            "max_fronteira": self.max_fronteira,
            "tempo_ms": round(self.tempo_ms, 3),
        }


def reconstruir(no: Node) -> tuple[list[Celula], list[int]]:
    caminho, acoes = [], []
    while no is not None:
        caminho.append(no.estado)
        if no.acao is not None:
            acoes.append(no.acao)
        no = no.pai
    return caminho[::-1], acoes[::-1]


def a_star(
    grid: list[str],
    inicio: Optional[Celula] = None,
    objetivo: Optional[Celula] = None,
    custo_fn: CustoFn = custo_unitario,
    heuristica_fn: HeuristicaFn = manhattan,
) -> Resultado:
    """Busca A* em grafo, sempre expandindo o nó de menor f = g + h.

    Sem `inicio`/`objetivo`, usa as células "S" e "G" do mapa.
    Empates em f são desfeitos pelo menor h e, depois, pela ordem de inserção.
    Se um nó já fechado for alcançado com g menor (heurística inconsistente),
    ele é reaberto, para que o custo devolvido seja o do melhor caminho achado.
    """
    grid = ler_mapa(grid)
    inicio = encontrar(grid, "S") if inicio is None else inicio
    objetivo = encontrar(grid, "G") if objetivo is None else objetivo
    if not transitavel(grid, inicio) or not transitavel(grid, objetivo):
        raise ValueError("início e objetivo precisam estar no mapa e fora de buracos")

    t0 = time.perf_counter()
    contador = 0  # desempate estável na heap
    raiz = Node(inicio, None, None, 0.0)
    h0 = heuristica_fn(inicio, objetivo)
    abertos = [(h0, h0, contador, raiz)]
    melhor_g = {inicio: 0.0}
    fechados: list[Celula] = []
    gerados, max_fronteira = 1, 1

    while abertos:
        _, _, _, no = heapq.heappop(abertos)
        if no.g > melhor_g[no.estado]:
            continue  # entrada velha: o estado já foi alcançado por um caminho melhor

        if no.estado == objetivo:
            caminho, acoes = reconstruir(no)
            return Resultado(True, caminho, acoes, no.g, len(fechados), gerados,
                             max_fronteira, (time.perf_counter() - t0) * 1000, fechados)

        fechados.append(no.estado)
        for acao, prox in vizinhos(grid, no.estado):
            g = no.g + custo_fn(no.estado, prox)
            if g < melhor_g.get(prox, math.inf):
                melhor_g[prox] = g
                h = heuristica_fn(prox, objetivo)
                contador += 1
                heapq.heappush(abertos, (g + h, h, contador, Node(prox, no, acao, g)))
                gerados += 1
        max_fronteira = max(max_fronteira, len(abertos))

    return Resultado(False, [], [], math.inf, len(fechados), gerados,
                     max_fronteira, (time.perf_counter() - t0) * 1000, fechados)


# ---------------------------------------------------------------------------
# Conferência do caminho e execução no Gymnasium
# ---------------------------------------------------------------------------

def validar_caminho(grid: list[str], resultado: Resultado,
                    custo_fn: CustoFn = custo_unitario) -> None:
    """Levanta AssertionError se o caminho não for válido para o mapa."""
    grid = ler_mapa(grid)
    if not resultado.encontrou:
        return
    caminho = resultado.caminho
    assert all(transitavel(grid, c) for c in caminho), "o caminho passa por buraco"
    assert len(resultado.acoes) == len(caminho) - 1
    custo = 0.0
    for (a, b), acao in zip(zip(caminho, caminho[1:]), resultado.acoes):
        di, dj = DESLOCAMENTOS[acao]
        assert (a[0] + di, a[1] + dj) == b, f"passo inválido {a} -> {b}"
        custo += custo_fn(a, b)
    assert math.isclose(custo, resultado.custo), f"custo {custo} != {resultado.custo}"


def executar_no_ambiente(grid: list[str], acoes: list[int], render_mode=None):
    """Executa as ações no FrozenLake-v1 não escorregadio.

    Só faz sentido quando a busca foi de "S" até "G", que são o início e o
    objetivo do ambiente. Devolve (recompensa total, terminou, estados visitados).
    """
    import gymnasium as gym

    grid = ler_mapa(grid)
    env = gym.make("FrozenLake-v1", desc=grid, is_slippery=False,
                   render_mode=render_mode, max_episode_steps=len(acoes) + 1)
    estado, _ = env.reset(seed=0)
    estados, recompensa, terminou = [estado], 0.0, False
    for acao in acoes:
        estado, r, terminou, truncou, _ = env.step(acao)
        estados.append(estado)
        recompensa += r
        if terminou or truncou:
            break
    env.close()
    return recompensa, terminou, estados


def gerar_gif(grid: list[str], acoes: list[int], arquivo, duracao_ms: int = 400) -> None:
    """Grava um GIF do agente seguindo as ações no ambiente (como no notebook base).

    Usa o Pillow, que já é dependência do matplotlib, no lugar do imageio.
    """
    import gymnasium as gym
    from PIL import Image

    grid = ler_mapa(grid)
    env = gym.make("FrozenLake-v1", desc=grid, is_slippery=False,
                   render_mode="rgb_array", max_episode_steps=len(acoes) + 1)
    env.reset(seed=0)
    quadros = [env.render()]
    for acao in acoes:
        _, _, terminou, truncou, _ = env.step(acao)
        quadros.append(env.render())
        if terminou or truncou:
            break
    env.close()
    imagens = [Image.fromarray(q) for q in quadros]
    imagens[0].save(arquivo, save_all=True, append_images=imagens[1:],
                    duration=duracao_ms, loop=0)


# ---------------------------------------------------------------------------
# Saída: texto e figura
# ---------------------------------------------------------------------------

def grid_com_caminho(grid: list[str], resultado: Resultado) -> str:
    """Mapa em texto com o caminho marcado por '*'."""
    linhas = [list(l) for l in ler_mapa(grid)]
    for i, j in resultado.caminho:
        if linhas[i][j] == "F":
            linhas[i][j] = "*"
    return "\n".join("".join(l) for l in linhas)


CORES_CELULAS = {"S": "#7fb3d5", "F": "#eaf2f8", "H": "#1b2631", "G": "#82e0aa"}


def desenhar(grid: list[str], resultado: Optional[Resultado] = None, ax=None,
             titulo: str = "", mostrar_expandidos: bool = True):
    """Desenha o mapa, as células expandidas (sombreadas) e o caminho."""
    import matplotlib.pyplot as plt
    from matplotlib.colors import to_rgb

    grid = ler_mapa(grid)
    n_lin, n_col = len(grid), len(grid[0])
    if ax is None:
        lado = max(3.0, min(10.0, 0.45 * max(n_lin, n_col)))
        _, ax = plt.subplots(figsize=(lado, lado))

    imagem = [[to_rgb(CORES_CELULAS.get(c, "#ffffff")) for c in linha] for linha in grid]
    if resultado is not None and mostrar_expandidos:
        expandidos = set(resultado.fechados)
        for i, j in expandidos:
            if grid[i][j] == "F":
                imagem[i][j] = to_rgb("#f9e79f")
    ax.imshow(imagem)

    if n_lin <= 16:
        for i, linha in enumerate(grid):
            for j, c in enumerate(linha):
                if c in "SGH":
                    ax.text(j, i, c, ha="center", va="center",
                            color="white" if c == "H" else "black", fontsize=9)

    if resultado is not None and resultado.encontrou:
        ys, xs = zip(*resultado.caminho)
        ax.plot(xs, ys, color="#c0392b", linewidth=2.5, marker="o", markersize=3)

    ax.set_xticks([x - 0.5 for x in range(1, n_col)], minor=True)
    ax.set_yticks([y - 0.5 for y in range(1, n_lin)], minor=True)
    ax.grid(which="minor", color="#aab7b8", linewidth=0.5)
    ax.tick_params(which="both", length=0, labelbottom=False, labelleft=False)
    if resultado is not None and not titulo:
        titulo = (f"custo {resultado.custo:g}, {resultado.expandidos} expandidos"
                  if resultado.encontrou else "sem caminho")
    ax.set_title(titulo)
    return ax
