# Game ai lab

Jogo 2D top-down em C++17 onde o mundo é representado por um **grafo de áreas**:
cada área da malha é um nó, as arestas são adjacências e apenas as áreas
próximas do jogador (no máximo quatro) ficam ativas e são atualizadas a cada quadro.

## Build

```sh
make            # configura e compila
make run        # compila e executa com a configuração padrão
make level      # compila e executa lendo assets/levels/level01.json
make random     # compila e executa com nível gerado aleatoriamente (semente 1)
make benchmark  # mede tempo de quadro (com/sem janela ativa) em benchmarks/frame_time.csv
make clean      # apaga build/
make rebuild    # apaga tudo e compila do zero
make help       # lista os alvos
```

O `Makefile` da raiz é só um atalho: quem manda na build é o `CMakeLists.txt`.
Direto pelo CMake, o equivalente é:

```sh
cmake -S . -B build
cmake --build build
./build/game
```

Requisitos: CMake 3.16 ou mais novo e um compilador com C++17. Sem dependências externas.

## Estrutura

```
include/game/                headers públicos
  Vector2.h                  vetor 2D, distância e interpolação
  Rectangle.h                retângulo alinhado aos eixos, com testes de colisão
  Character.h                base com health, takeDamage, heal e isDead
  Player.h                   jogador: input e movimento
  NonPlayerCharacter.h       inimigo que persegue o jogador e causa dano por contato
  Item.h                     coletáveis (HEALTH e AMMUNITION)
  Area.h                     nó do grafo: limites, inimigos, itens e estado
  WorldGraph.h               lista de adjacência, janela de áreas ativas, carga sob demanda
  Viewport.h                 câmera, que não conhece o WorldGraph
  LevelConfiguration.h       todos os parâmetros de balanceamento em uma struct
  Renderer.h                 desenho no terminal
  Game.h                     laço principal, cronômetro e condições de vitória e derrota
  Benchmark.h                medição de tempo de quadro com/sem janela ativa
src/                         implementações (Item.h e Rectangle.h não têm .cpp)
assets/levels/               configurações de nível em JSON
benchmarks/                  resultados de `make benchmark` em CSV
```

## Estado atual

Jogável de ponta a ponta: o jogador se move, os inimigos perseguem e causam
dano, itens curam ou aplicam dano em área, e tudo aparece no terminal. `make
random` gera um nível aleatório por semente; `make level` lê um nível de
`assets/levels/`. `make benchmark` mede e grava o ganho de desempenho da
janela ativa contra a linha de base ingênua. Detalhes de decisão e do que
ainda falta (testes automatizados) estão em `PLAN.md`, um nível acima
deste repositório (fora do controle de versão do jogo).
