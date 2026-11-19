"""Testes do A*. Rodar com `python teste_astar.py` (ou `pytest`)."""

import math

from gymnasium.envs.toy_text.frozen_lake import generate_random_map

from astar import (a_star, custo_unitario, euclidiana, executar_no_ambiente,
                   heuristica_nula, manhattan, validar_caminho)

MAPA_AULA = ["SFFF", "FHFH", "FFFH", "HFFG"]  # 4x4 padrão do FrozenLake-v1


def teste_exemplo_da_aula():
    """Slides 18 a 32: (0,0) -> (3,3), Manhattan, custo 1, termina com g = 6."""
    r = a_star(MAPA_AULA, (0, 0), (3, 3), custo_unitario, manhattan)
    assert r.encontrou and r.custo == 6
    validar_caminho(MAPA_AULA, r)
    recompensa, terminou, _ = executar_no_ambiente(MAPA_AULA, r.acoes)
    assert terminou and recompensa == 1.0


MAPA_BASE = ["FFFFFFFS", "FHFHHHHH", "FHFFHFFF", "FHHFFFHF",  # mapa 8x8 do notebook
             "FHFHHHFF", "FHFFFHFH", "FHFHFHHH", "FFFHFFFG"]  # base (Pratica_2_Completa)


def teste_mapa_do_notebook_base():
    """O notebook base encontra um caminho de 25 passos, de (0,7) a (7,7)."""
    r = a_star(MAPA_BASE)
    assert r.caminho[0] == (0, 7) and r.caminho[-1] == (7, 7) and r.custo == 25
    validar_caminho(MAPA_BASE, r)
    recompensa, terminou, _ = executar_no_ambiente(MAPA_BASE, r.acoes)
    assert terminou and recompensa == 1.0


def teste_igual_a_dijkstra_em_mapas_aleatorios():
    """Com heurística admissível, o custo é o mesmo de h = 0 (Dijkstra)."""
    for tamanho in (4, 8, 16, 32):
        for semente in range(10):
            mapa = generate_random_map(size=tamanho, p=0.8, seed=semente)
            base = a_star(mapa, heuristica_fn=heuristica_nula)
            for h in (manhattan, euclidiana):
                r = a_star(mapa, heuristica_fn=h)
                assert r.encontrou == base.encontrou
                assert math.isclose(r.custo, base.custo)
                assert r.expandidos <= base.expandidos
                validar_caminho(mapa, r)
            recompensa, terminou, _ = executar_no_ambiente(mapa, base.acoes)
            assert terminou and recompensa == 1.0


def teste_sem_caminho():
    mapa = ["SFH", "FHF", "HFG"]
    r = a_star(mapa)
    assert not r.encontrou and r.custo == math.inf and r.caminho == []
    assert r.expandidos == 3  # só S, (0,1) e (1,0) são alcançáveis


def teste_geradores_de_cenario():
    from cenarios import (existe_caminho, manhattan_ponderada, mapa_armadilha_u,
                          mapa_labirinto, mapa_objetivo_isolado, mapa_parede, montar_mapa,
                          posicionar)
    # a BFS de existe_caminho concorda com o A* em mapas sem filtro
    for k in range(30):
        mapa = montar_mapa(12, p_livre=0.7, semente=k)
        assert existe_caminho(mapa) == a_star(mapa).encontrou
    # posicionar mantém os buracos e move S e G
    base = montar_mapa(8, p_livre=0.8, semente=1)
    novo = posicionar(base, (3, 3), (7, 0))
    assert novo[3][3] == "S" and novo[7][0] == "G" and novo[0][0] in "FH"
    assert sum(l.count("H") for l in novo) <= sum(l.count("H") for l in base)
    # padrões: custos conhecidos
    assert a_star(mapa_parede(16, 8)).custo == 15
    assert a_star(mapa_parede(16, 0)).custo == 31
    assert a_star(mapa_armadilha_u(16)).custo == 24
    assert not a_star(mapa_objetivo_isolado(16)).encontrou
    lab = mapa_labirinto(15, semente=3)
    assert existe_caminho(lab)
    # A* ponderado nunca passa de w vezes o ótimo
    for k in range(20):
        mapa = generate_random_map(size=16, p=0.7, seed=k)
        otimo = a_star(mapa, heuristica_fn=heuristica_nula).custo
        for w in (1.5, 2, 5):
            assert a_star(mapa, heuristica_fn=manhattan_ponderada(w)).custo <= w * otimo


if __name__ == "__main__":
    for nome, teste in list(globals().items()):
        if nome.startswith("teste_") and callable(teste):
            teste()
            print(f"ok  {nome}")
