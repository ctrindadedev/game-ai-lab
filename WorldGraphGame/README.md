### WorldGraphGame

<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 5px;">
 <img align="center" alt="C++" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cplusplus/cplusplus-original.svg" />
 <img align="center" alt="CMake" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cmake/cmake-original.svg" />
</div>

### Sobre o projeto

Atividade 1 de [DIM0126 - Inteligência Artificial para Jogos I](../README.md). Jogo top-down em C++17 que roda no terminal. O mapa é dividido em uma malha de
áreas, e cada área é um nó de um grafo cujas arestas ligam áreas vizinhas.
Só as áreas perto do jogador (no máximo quatro) ficam ativas e são
atualizadas a cada quadro. As outras não gastam processamento e, depois de um
tempo inativas, são liberadas da memória.

O objetivo é sobreviver até o tempo acabar, fugindo dos inimigos (`e`) e
pegando itens de vida (`h`) e de munição (`a`, que causa dano em área).

## Compilando e rodando

Precisa de CMake 3.16+ e um compilador com suporte a C++17.

```sh
make            # compila
make run        # roda com a configuração padrão
make level      # roda com assets/levels/level01.json
make random     # roda com um nível aleatório (semente 1)
make benchmark  # mede o tempo de quadro e salva em benchmarks/frame_time.csv
make clean      # apaga build/
```

O Makefile só chama o CMake. Sem ele:

```sh
cmake -S . -B build
cmake --build build
./build/game [arquivo.json | --random <semente>]
```

## Controles

- `W A S D` ou setas: mover
- `Q E Z C`: diagonais
- espaço: parar
- `Ctrl+C`: sair

O jogador continua andando na última direção escolhida até você apertar
outra tecla ou o espaço. A câmera acompanha o jogador, então o `@` fica perto
do centro da tela. Quem se move é a grade de fundo: os `.` marcam o chão e
`| - +` marcam as bordas entre as áreas.

## Níveis

Os níveis ficam em `assets/levels/` como JSON com campos numéricos: tamanho
da malha e das áreas, número de inimigos e itens por área, velocidades, dano,
tempo de sobrevivência, semente etc. Campo que não aparece no arquivo fica
com o valor padrão de `LevelConfiguration`.

## Organização do código

- `include/game/`: headers
- `src/`: implementações
- `assets/levels/`: níveis
- `benchmarks/`: resultados do `make benchmark`

As peças principais são `WorldGraph` (grafo de áreas e controle de quais
estão ativas), `Area` (inimigos e itens de uma área), `Game` (laço principal
com passo fixo de 1/60 s) e `Renderer` (desenho no terminal).
