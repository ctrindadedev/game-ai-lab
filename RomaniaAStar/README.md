# A* no mapa da Romênia

Execução passo a passo do algoritmo A* no grafo do último slide da aula 26
(slides *4.1 — Métodos para deslocamento: pathfinding*), saindo de **Arad**
e chegando a **Bucharest**.

## Atividade

> Exercício sobre deslocamento no mundo — Execução do algoritmo A* em um grafo.
> Tarefa criada para submeter o passo a passo da execução do algoritmo A* no
> grafo do último slide da aula 26.

- **Prazo:** 16/11/2026 a 23/11/2026, 23h59 (SIGAA)
- **Heurística h:** distância em linha reta de cada cidade até Bucharest (tabela do slide)
- **Função de avaliação:** f(n) = g(n) + h(n), em que g é a distância percorrida desde Arad
- **Pseudocódigo de referência:** o dos slides, com as listas `OPEN` (abertos) e `CLOSED` (fechados)

## Arquivos

| Arquivo | Conteúdo | Situação |
|---|---|---|
| `romenia.py` | Estradas, distâncias e tabela h transcritas do slide | pronto |
| `astar_romenia.py` | A* que imprime OPEN, CLOSED e f/g/h a cada iteração, para conferir a execução manual | pronto |
| `passo_a_passo.tex` | Documento de entrega com a execução iteração por iteração | pronto |

## Como rodar

```bash
python romenia.py        # confere a transcrição do grafo
python astar_romenia.py  # A* passo a passo, com OPEN e CLOSED a cada iteração
pdflatex passo_a_passo.tex
```

## Resultado

Caminho **Arad → Sibiu → Rimnicu Vilcea → Pitesti → Bucharest**, com custo **418 km**,
em 6 iterações (5 expansões). Bucharest entra em `OPEN` pela primeira vez via Fagaras
(g = 450), mas a busca só termina quando ele sai de `OPEN`; antes disso, a expansão de
Pitesti (f = 417) encontra o caminho de 418 e atualiza o registro.
