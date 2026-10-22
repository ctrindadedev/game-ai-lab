### TicTacToeAI

<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 5px;">
 <img align="center" alt="C++" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cplusplus/cplusplus-original.svg" />
 <img align="center" alt="CMake" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/cmake/cmake-original.svg" />
</div>

### Sobre o projeto

Atividade 4 de [DIM0126 - Inteligência Artificial para Jogos I](../README.md). Jogo da velha em C++17 no terminal com dois NPCs: um usa Minimax e o outro usa Monte Carlo Tree Search (MCTS).

A estrutura segue o framework de jogos do livro *Artificial Intelligence: A Modern Approach* (Russell & Norvig), traduzido do [aima-python](https://github.com/aimacode/aima-python) para C++:

- `Game` é a interface comum: `actions`, `result`, `utility`, `terminalTest`, `toMove`, `display` e `playGame` (no Python, `terminal_test`, `to_move` e `play_game`).
- `TicTacToe` implementa `Game`. X é o primeiro jogador (Max), e a utilidade é +1 se X vence, -1 se O vence e 0 no empate.
- Um jogador (`Player`) é uma função `player(game, state)` que devolve uma jogada. Por isso o Minimax e o MCTS podem jogar entre si ou contra uma pessoa.

A dificuldade do Minimax é a profundidade `d` de `alphabetaCutoffSearch`, e a do MCTS é o número de iterações `N` de `monteCarloTreeSearch`. Como no original, a busca é cortada quando a profundidade passa de `d` e, nesse ponto, o estado vale a sua utilidade (0 se o jogo ainda não acabou).

### Enunciado

Implementação de NPC utilizando Minimax, Monte Carlo Tree Search (MCTS) ou algoritmo evolutivo para o Tic-Tac-Toe (jogo da velha).

### Compilando e rodando

Precisa de CMake 3.16+ e um compilador com suporte a C++17.

```sh
make            # compila
make run        # compila e executa
make clean      # apaga build/
```

O Makefile só chama o CMake. Sem ele:

```sh
cmake -S . -B build
cmake --build build
./build/tictactoe [jogadorX] [jogadorO]
```

### Jogadores

X sempre começa. Sem argumentos, é `humano` contra `minimax`.

- `humano`: joga pelo teclado, digitando linha e coluna (por exemplo `2 2`)
- `aleatorio`: jogada válida ao acaso
- `minimax`: Minimax com poda alfa-beta até o fim do jogo (imbatível)
- `minimax:<d>`: Minimax com poda alfa-beta cortado na profundidade `d`
- `mcts`: Monte Carlo Tree Search com 1000 iterações
- `mcts:<N>`: Monte Carlo Tree Search com `N` iterações

```sh
./build/tictactoe humano minimax:1
./build/tictactoe minimax aleatorio
./build/tictactoe mcts:100 minimax:2
```

As casas são `(linha, coluna)`, de `(1, 1)` no canto superior esquerdo até `(3, 3)`. Cada jogada aparece no terminal com o tabuleiro depois dela, por exemplo `O (mcts:100) joga (2, 2)`.

### Organização do código

- `include/game/`: headers
- `src/`: implementações

Os arquivos seguem as partes do aima-python:

- `Game.h` / `Game.cpp`: `GameState`, `Player` e a interface `Game` com `playGame`
- `TicTacToe.h` / `TicTacToe.cpp`: o jogo da velha
- `Minimax.h` / `Minimax.cpp`: `minimaxDecision`, `alphabetaSearch` e `alphabetaCutoffSearch`
- `MonteCarloTreeSearch.h` / `MonteCarloTreeSearch.cpp`: `monteCarloTreeSearch`
- `Players.h` / `Players.cpp`: `queryPlayer`, `randomPlayer`, `alphabetaPlayer` e `mctsPlayer`
- `main.cpp`: escolhe os jogadores pela linha de comando e roda a partida

A única diferença em relação ao original é o `queryPlayer`: ele pede a jogada de novo quando ela é inválida. No Python, uma jogada inválida não muda o estado e passa a vez.
