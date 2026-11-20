"""Custos variáveis para o A* no Lago Congelado, com o Uber como inspiração.

Cada mapa ganha três camadas do mesmo tamanho do grid:

    terreno      custo de andar no piso: asfalto 1, pedregulho 2, barro 3, areia 4
    inseguranca  penalidade pelas ocorrências policiais do bairro
    trafego      penalidade pelo trânsito, que muda com o horário

As camadas são geradas por bairros (regiões de Voronoi), e não célula a célula,
então ruas vizinhas costumam ter o mesmo piso, o mesmo risco e o mesmo trânsito.
Cada camada usa a sua própria divisão em bairros.

O custo de um passo é o custo de **entrar** na célula de destino:

    custo = 1 + w_terreno·(terreno - 1) + w_seguranca·inseguranca + w_trafego·trafego

Com pesos >= 0 e penalidades >= 0, todo passo custa pelo menos 1, que é o custo
da aula (asfalto, sem risco e sem trânsito).
"""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Optional

import numpy as np

from astar import (Celula, a_star, executar_no_ambiente, heuristica_nula, ler_mapa,
                   manhattan, validar_caminho)

TERRENOS = {1: "asfalto", 2: "pedregulho", 3: "barro", 4: "areia"}
PROB_TERRENO = [0.45, 0.25, 0.18, 0.12]  # a maior parte da cidade é asfalto

# Penalidade de trânsito = base + escala · intensidade (intensidade entre 0 e 1).
# No pico até a rua vazia tem uma penalidade mínima (semáforos, conversões).
HORARIOS = {
    "pico": (0.5, 3.0),
    "normal": (0.0, 1.5),
    "madrugada": (0.0, 0.3),
}

# Com chuva o barro vira atoleiro, o pedregulho escorrega e o trânsito piora.
# O asfalto e a areia (que fica mais firme molhada) quase não mudam.
CHUVA_TERRENO = {1: 1.0, 2: 2.5, 3: 6.0, 4: 4.0}
CHUVA_TRAFEGO = 1.5

OCORRENCIAS_POR_PONTO = 5   # cada 5 ocorrências no bairro somam 1 à penalidade
MAX_INSEGURANCA = 5.0


# ---------------------------------------------------------------------------
# Camadas
# ---------------------------------------------------------------------------

def regioes_voronoi(n_lin: int, n_col: int, n_regioes: int, rng: np.random.Generator) -> np.ndarray:
    """Divide o grid em bairros: cada célula fica com a semente mais próxima."""
    si = rng.uniform(0, n_lin, n_regioes)
    sj = rng.uniform(0, n_col, n_regioes)
    ii, jj = np.mgrid[0:n_lin, 0:n_col]
    dist = (ii[..., None] - si) ** 2 + (jj[..., None] - sj) ** 2
    return dist.argmin(axis=2)


@dataclass
class Camadas:
    mapa: list[str]
    tipo_terreno: np.ndarray   # 1..4 (asfalto, pedregulho, barro, areia)
    terreno: np.ndarray        # custo do piso (igual ao tipo, a não ser com chuva)
    ocorrencias: np.ndarray    # ocorrências policiais do bairro
    inseguranca: np.ndarray    # penalidade de segurança
    intensidade: np.ndarray    # trânsito entre 0 e 1, independente do horário
    trafego: np.ndarray        # penalidade de trânsito no horário escolhido
    avenidas: np.ndarray       # máscara das avenidas
    horario: str
    clima: str


def gerar_camadas(mapa: list[str], semente: int, horario: str = "pico", clima: str = "seco",
                  celulas_por_bairro: int = 20, n_avenidas: Optional[int] = None) -> Camadas:
    """Gera terreno, insegurança e tráfego por bairros, com semente.

    `horario` ("pico", "normal", "madrugada") e `clima` ("seco", "chuva") só
    mudam a conversão em penalidade: com a mesma semente, a cidade (bairros,
    pisos, ocorrências e avenidas) é a mesma, o que permite comparar os cenários.

    As avenidas são linhas e colunas inteiras de asfalto com trânsito máximo:
    rápidas de madrugada e congestionadas no pico.
    """
    if horario not in HORARIOS:
        raise ValueError(f"horário desconhecido: {horario!r}")
    if clima not in ("seco", "chuva"):
        raise ValueError(f"clima desconhecido: {clima!r}")
    mapa = ler_mapa(mapa)
    n_lin, n_col = len(mapa), len(mapa[0])
    n_bairros = max(2, round(n_lin * n_col / celulas_por_bairro))
    # Uma semente independente por camada, para que piso, risco e trânsito
    # não andem juntos (senão todos os perfis escolheriam a mesma rota).
    rng_t, rng_s, rng_tr = (np.random.default_rng(s)
                            for s in np.random.SeedSequence(semente).spawn(3))

    # Terreno: um tipo de piso por bairro.
    bairros = regioes_voronoi(n_lin, n_col, n_bairros, rng_t)
    tipo = rng_t.choice([1, 2, 3, 4], size=n_bairros, p=PROB_TERRENO)[bairros]

    # Segurança: bairros maiores, ocorrências por Poisson com média variando
    # bastante entre bairros (poucos bairros concentram a maior parte).
    bairros = regioes_voronoi(n_lin, n_col, max(2, n_bairros // 2), rng_s)
    media = rng_s.gamma(shape=1.2, scale=8.0, size=bairros.max() + 1)
    ocorrencias = rng_s.poisson(media)[bairros]
    inseguranca = np.minimum(ocorrencias / OCORRENCIAS_POR_PONTO, MAX_INSEGURANCA)

    # Tráfego: intensidade por bairro mais as avenidas.
    bairros = regioes_voronoi(n_lin, n_col, n_bairros, rng_tr)
    intensidade = rng_tr.uniform(0.0, 0.6, size=n_bairros)[bairros]
    if n_avenidas is None:
        n_avenidas = max(1, n_lin // 10)
    avenidas = np.zeros((n_lin, n_col), dtype=bool)
    if n_lin > 2 and n_col > 2:  # avenidas não passam pelas bordas
        for i in rng_tr.choice(np.arange(1, n_lin - 1), size=min(n_avenidas, n_lin - 2), replace=False):
            avenidas[i, :] = True
        for j in rng_tr.choice(np.arange(1, n_col - 1), size=min(n_avenidas, n_col - 2), replace=False):
            avenidas[:, j] = True
    intensidade = np.where(avenidas, 1.0, intensidade)
    tipo = np.where(avenidas, 1, tipo)

    base, escala = HORARIOS[horario]
    trafego = base + escala * intensidade
    terreno = tipo.astype(float)
    if clima == "chuva":
        terreno = np.vectorize(CHUVA_TERRENO.get)(tipo).astype(float)
        trafego = trafego * CHUVA_TRAFEGO

    return Camadas(mapa, tipo, terreno, ocorrencias, inseguranca.astype(float),
                   intensidade, trafego, avenidas, horario, clima)


# ---------------------------------------------------------------------------
# Função de custo e perfis
# ---------------------------------------------------------------------------

class Custo:
    """custo_fn(origem, destino) = custo de entrar em `destino`.

    Guarda a grade de custos (`grade`) para as heurísticas e os gráficos
    usarem exatamente os mesmos números da busca.
    """

    def __init__(self, camadas: Camadas, w_terreno: float, w_seguranca: float,
                 w_trafego: float, nome: str = ""):
        if min(w_terreno, w_seguranca, w_trafego) < 0:
            raise ValueError("os pesos precisam ser >= 0 para o custo ficar >= 1")
        self.camadas = camadas
        self.pesos = (w_terreno, w_seguranca, w_trafego)
        self.nome = nome
        self.grade = (1.0 + w_terreno * (camadas.terreno - 1) + w_seguranca * camadas.inseguranca
                      + w_trafego * camadas.trafego)
        self._lista = self.grade.tolist()  # acesso rápido e em float puro
        livres = np.array([[c != "H" for c in linha] for linha in camadas.mapa])
        self.minimo = float(self.grade[livres].min())
        self.media = float(self.grade[livres].mean())
        self.maximo = float(self.grade[livres].max())

    def __call__(self, origem: Celula, destino: Celula) -> float:
        return self._lista[destino[0]][destino[1]]


def criar_custo(camadas: Camadas, w_terreno: float = 1.0, w_seguranca: float = 1.0,
                w_trafego: float = 1.0, nome: str = "") -> Custo:
    """Fábrica da custo_fn que combina as três camadas com os pesos dados."""
    return Custo(camadas, w_terreno, w_seguranca, w_trafego, nome)


PERFIS = {
    # Tempo de viagem: piso e trânsito, sem olhar o risco.
    "mais rápido": dict(w_terreno=1.0, w_seguranca=0.0, w_trafego=1.0),
    # Evita bairros perigosos e aceita andar mais e pegar piso pior.
    "mais seguro": dict(w_terreno=0.3, w_seguranca=2.0, w_trafego=0.3),
    # Meio-termo entre os dois.
    "equilibrado": dict(w_terreno=1.0, w_seguranca=0.7, w_trafego=0.7),
}


def custo_do_perfil(camadas: Camadas, perfil: str) -> Custo:
    return criar_custo(camadas, **PERFIS[perfil], nome=perfil)


def exposicao(camadas: Camadas, resultado) -> dict:
    """Soma de cada camada ao longo do caminho (sem contar a célula inicial).

    Mostra do que cada rota "paga": piso, risco e trânsito, independentemente
    dos pesos do perfil que a escolheu.
    """
    passos = resultado.caminho[1:]
    soma = lambda grade: float(sum(grade[c] for c in passos))
    return {"terreno": soma(camadas.terreno), "inseguranca": soma(camadas.inseguranca),
            "trafego": soma(camadas.trafego)}


# ---------------------------------------------------------------------------
# Heurísticas
# ---------------------------------------------------------------------------

def manhattan_escalada(fator: float, nome: str = ""):
    """h = fator · Manhattan.

    Com fator <= custo mínimo de um passo, nunca superestima (admissível e
    consistente). Acima disso pode superestimar.
    """
    def h(celula: Celula, objetivo: Celula) -> float:
        return fator * manhattan(celula, objetivo)
    h.__name__ = nome or f"manhattan_x{fator:g}"
    return h


def heuristicas_de_custo(custo: Custo, w: float = 2.0) -> dict:
    """Heurísticas derivadas da grade de custos.

    - manhattan: admissível (custo >= 1), mas fraca quando os custos são altos;
    - manhattan × cmin: admissível e a mais forte das admissíveis daqui;
    - manhattan × média, × máx e w·(manhattan × cmin): podem superestimar.
    """
    return {
        "manhattan": manhattan,
        "manhattan × cmin": manhattan_escalada(custo.minimo, "manhattan_x_cmin"),
        "manhattan × média": manhattan_escalada(custo.media, "manhattan_x_media"),
        "manhattan × máx": manhattan_escalada(custo.maximo, "manhattan_x_max"),
        f"{w:g} × manhattan × cmin": manhattan_escalada(w * custo.minimo, "manhattan_x_w_cmin"),
    }


ADMISSIVEIS = ("nula (Dijkstra)", "manhattan", "manhattan × cmin")


# ---------------------------------------------------------------------------
# Execução dos experimentos
# ---------------------------------------------------------------------------

def rodar_custo(mapa: list[str], custo: Custo, heuristicas: dict,
                inicio: Optional[Celula] = None, objetivo: Optional[Celula] = None,
                conferir_no_gym: bool = True) -> list[dict]:
    """Como `cenarios.rodar`, mas com custo variável.

    Cada caminho é validado com a mesma custo_fn, comparado com Dijkstra
    (`otimo`) e, quando a busca vai de S a G, executado no Gymnasium. Com uma
    heurística admissível, o custo precisa ser igual ao de Dijkstra.
    """
    otimo = a_star(mapa, inicio, objetivo, custo_fn=custo, heuristica_fn=heuristica_nula)
    linhas = []
    for nome, h in {"nula (Dijkstra)": heuristica_nula, **heuristicas}.items():
        r = otimo if h is heuristica_nula else a_star(mapa, inicio, objetivo, custo_fn=custo,
                                                      heuristica_fn=h)
        validar_caminho(mapa, r, custo_fn=custo)
        if nome in ADMISSIVEIS:
            assert math.isclose(r.custo, otimo.custo), f"{nome}: custo diferente de Dijkstra"
        if conferir_no_gym and r.encontrou and inicio is None and objetivo is None:
            recompensa, terminou, _ = executar_no_ambiente(mapa, r.acoes)
            assert terminou and recompensa == 1.0, "o caminho não chegou ao G no Gymnasium"
        excesso = (r.custo / otimo.custo - 1) * 100 if otimo.encontrou else math.nan
        linhas.append({"heuristica": nome, **r.metricas(), "otimo": otimo.custo,
                       "excesso_%": excesso, "cmin": custo.minimo, "resultado": r})
    return linhas


# ---------------------------------------------------------------------------
# Figura
# ---------------------------------------------------------------------------

def desenhar_camada(mapa: list[str], grade: np.ndarray, ax, resultados=(), cmap="YlOrRd",
                    titulo: str = "", vmin=None, vmax=None, cores=("#c0392b",), rotulos=()):
    """Mapa de calor de uma camada, buracos em escuro e rotas por cima.

    `resultados` aceita várias rotas (uma cor por rota, legenda por `rotulos`).
    Devolve a imagem, para a barra de cores.
    """
    import matplotlib.patheffects as pe
    import matplotlib.pyplot as plt

    mapa = ler_mapa(mapa)
    buracos = np.array([[c == "H" for c in linha] for linha in mapa])
    cm = plt.get_cmap(cmap).copy()
    cm.set_bad("#1b2631")
    img = ax.imshow(np.ma.masked_where(buracos, grade), cmap=cm, vmin=vmin, vmax=vmax)

    for k, r in enumerate(resultados):
        if r is None or not r.encontrou:
            continue
        ys, xs = zip(*r.caminho)
        desloc = 0.12 * (k - (len(resultados) - 1) / 2) if len(resultados) > 1 else 0.0
        ax.plot([x + desloc for x in xs], [y + desloc for y in ys], color=cores[k % len(cores)],
                linewidth=2.2,
                label=rotulos[k] if k < len(rotulos) else None,
                path_effects=[pe.Stroke(linewidth=4, foreground="white"), pe.Normal()])

    for i, linha in enumerate(mapa):
        for j, c in enumerate(linha):
            if c in "SG":
                ax.text(j, i, c, ha="center", va="center", fontsize=9, fontweight="bold",
                        color="black", bbox=dict(boxstyle="round,pad=0.15", fc="white",
                                                 ec="none", alpha=0.85))
    ax.set_xticks([])
    ax.set_yticks([])
    ax.set_title(titulo, loc="left", fontsize=10)
    return img
