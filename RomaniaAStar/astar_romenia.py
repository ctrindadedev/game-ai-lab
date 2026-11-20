"""A* passo a passo no grafo da Romênia, de Arad até Bucharest.

Segue o pseudocódigo dos slides da aula 26, com as listas OPEN (nós abertos,
a fronteira) e CLOSED (nós já expandidos) e a avaliação f = g + h:

    OPEN <- {início};  CLOSED <- {}
    enquanto OPEN não estiver vazia:
        n <- nó de OPEN com menor f
        se n é o objetivo: devolve o caminho
        move n de OPEN para CLOSED
        para cada vizinho m de n, com g' = g(n) + custo(n, m):
            se m está em CLOSED com g <= g': ignora
            se m está em OPEN com g <= g':   ignora
            senão: g(m) <- g', pai(m) <- n e m entra (ou fica) em OPEN

Empates em f são desfeitos pelo menor h e, depois, pela ordem em que o nó
entrou em OPEN. Os dados do grafo vêm de romenia.py.

A cada iteração o script imprime o nó expandido, os filhos gerados (g, h, f,
pai e o que aconteceu com cada um) e como ficaram OPEN e CLOSED. Serve para
conferir a execução feita à mão no passo_a_passo.tex.

    python astar_romenia.py
"""

from __future__ import annotations

from dataclasses import dataclass, replace
from typing import Optional

from romenia import H_LINHA_RETA, INICIO, OBJETIVO, vizinhos


@dataclass
class Registro:
    """Entrada de OPEN ou CLOSED: cidade, custo desde o início e pai."""

    cidade: str
    g: int
    pai: Optional[str]

    @property
    def h(self) -> int:
        return H_LINHA_RETA[self.cidade]

    @property
    def f(self) -> int:
        return self.g + self.h


@dataclass
class Filho:
    """Vizinho gerado na expansão de um nó e o que a busca fez com ele."""

    cidade: str
    g: int
    h: int
    f: int
    pai: str
    acao: str  # "entra em OPEN", "atualiza OPEN", "ignorado: ..." etc.


@dataclass
class Iteracao:
    numero: int
    expandido: Registro
    objetivo: bool                  # True se o nó retirado de OPEN é o objetivo
    filhos: list[Filho]
    open_: list[Registro]           # OPEN ao fim da iteração, em ordem de f
    closed: list[str]               # CLOSED ao fim da iteração, em ordem de entrada


def ordem(registro: Registro) -> tuple[int, int]:
    return (registro.f, registro.h)


def foto(open_: dict[str, Registro]) -> list[Registro]:
    """Cópia de OPEN em ordem de f, para o registro não mudar nas iterações seguintes."""
    return [replace(r) for r in sorted(open_.values(), key=ordem)]


def a_estrela(inicio: str = INICIO, objetivo: str = OBJETIVO):
    """Roda o A* e devolve (caminho, custo, lista de iterações)."""
    adj = vizinhos()
    # dicts guardam a ordem de inserção, que serve de último critério de desempate
    open_: dict[str, Registro] = {inicio: Registro(inicio, 0, None)}
    closed: dict[str, Registro] = {}
    iteracoes: list[Iteracao] = []

    while open_:
        n = min(open_.values(), key=ordem)  # min devolve o primeiro em caso de empate
        del open_[n.cidade]

        if n.cidade == objetivo:
            iteracoes.append(Iteracao(len(iteracoes) + 1, n, True, [],
                                      foto(open_), list(closed)))
            return reconstruir(n, closed), n.g, iteracoes

        closed[n.cidade] = n
        filhos = []
        for m, custo in adj[n.cidade].items():
            g = n.g + custo
            h = H_LINHA_RETA[m]
            if m in closed and closed[m].g <= g:
                acao = f"ignorado: já está em CLOSED com g = {closed[m].g}"
            elif m in open_ and open_[m].g <= g:
                acao = f"ignorado: já está em OPEN com g = {open_[m].g}"
            else:
                if m in closed:
                    acao = f"sai de CLOSED e volta a OPEN (g antigo {closed.pop(m).g})"
                elif m in open_:
                    acao = f"atualiza OPEN (g antigo {open_[m].g}, pai antigo {open_[m].pai})"
                    open_[m].g, open_[m].pai = g, n.cidade
                else:
                    acao = "entra em OPEN"
                if m not in open_:
                    open_[m] = Registro(m, g, n.cidade)
            filhos.append(Filho(m, g, h, g + h, n.cidade, acao))

        iteracoes.append(Iteracao(len(iteracoes) + 1, n, False, filhos,
                                  foto(open_), list(closed)))

    return [], None, iteracoes


def reconstruir(no: Registro, closed: dict[str, Registro]) -> list[str]:
    """Segue os pais a partir do objetivo; os pais já estão todos em CLOSED."""
    caminho = [no.cidade]
    while no.pai is not None:
        no = closed[no.pai]
        caminho.append(no.cidade)
    return caminho[::-1]


# ---------------------------------------------------------------------------
# Saída em texto
# ---------------------------------------------------------------------------

def formatar_open(registros: list[Registro]) -> str:
    if not registros:
        return "(vazia)"
    return ", ".join(f"{r.cidade}({r.f})" for r in registros)


def imprimir(iteracoes: list[Iteracao]) -> None:
    for it in iteracoes:
        n = it.expandido
        pai = n.pai or "-"
        print(f"=== Iteração {it.numero} ===")
        print(f"Sai de OPEN: {n.cidade}  (g = {n.g}, h = {n.h}, f = {n.f}, pai = {pai})")
        if it.objetivo:
            print(f"{n.cidade} é o objetivo: fim da busca.")
        else:
            print(f"{n.cidade} vai para CLOSED e é expandido:")
            print(f"  {'filho':<16}{'g':>5}{'h':>5}{'f':>5}  {'pai':<16}o que acontece")
            for c in it.filhos:
                print(f"  {c.cidade:<16}{c.g:>5}{c.h:>5}{c.f:>5}  {c.pai:<16}{c.acao}")
        print(f"OPEN   = {formatar_open(it.open_)}")
        print(f"CLOSED = {', '.join(it.closed) or '(vazia)'}")
        print()


def main() -> None:
    caminho, custo, iteracoes = a_estrela()
    imprimir(iteracoes)
    if caminho:
        print("Caminho: " + " -> ".join(caminho))
        print(f"Custo total: {custo} km")
        print(f"Iterações: {len(iteracoes)} ({len(iteracoes) - 1} expansões)")
    else:
        print(f"Não há caminho de {INICIO} até {OBJETIVO}.")


if __name__ == "__main__":
    main()
