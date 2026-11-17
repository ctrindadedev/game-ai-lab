# Atividade — Experimentação do algoritmo A* no cenário do Lago Congelado

**Disciplina:** Inteligência Artificial para Jogos I
**Professor:** Charles Madeira
**Origem:** Aula 27 — *Métodos para Movimentação no Mundo* (pathfinding)
**Tipo de tarefa:** Exercício sobre deslocamento no mundo, entregue pelo SIGAA

---

## 1. Descrição da atividade

Tarefa criada para submeter um **relatório redigido**, com o objetivo de fazer uma **análise crítica** sobre os experimentos com o algoritmo **A\*** no ambiente do **Lago Congelado**, conforme indicado nos últimos slides da aula 27.

### Exigências de entrega

| Item | Exigência |
|---|---|
| Formato do relatório | Seguir o **documento-modelo disponibilizado na turma virtual do SIGAA** |
| Código implementado | Disponibilizar no relatório por **link aberto** (garantir que o acesso funcione) |
| Onde entregar | Atividade criada no **SIGAA** |
| Base para o código | Notebook Python de exemplo ("Lago congelado no Colab"), a ser adaptado |

> No slide final, o pedido é postar no SIGAA o **link do notebook adaptado**, garantindo que esteja acessível. O PDF contém um link do Google Drive no slide 36 (`https://drive.google.com/file/d/17MqiG-smtxgpHkODRRGZcZO0rxdafAzc/view`), que provavelmente corresponde ao notebook base. Confirme isso abrindo o link.

---

## 2. O que se pede nos experimentos (slides 33 a 36)

O que foi mostrado em aula se limitou à **distância Manhattan** em um cenário simples. Agora é preciso **adicionar variações** para explorar o funcionamento da busca A\*, em dois eixos.

### 2.1 Configurações do cenário

- **Tamanho do grid** (ex.: 8x8, 16x16, ...)
- **Posição inicial** variável do agente
- **Posição do objetivo** variável
- **Posições dos buracos** variadas

### 2.2 Variáveis do cálculo de custo

Tendo o **Uber como inspiração**, a ideia é tornar o estado do ambiente mais complexo, para que o cálculo do custo da rota fique **mais realista**. A distância sozinha pode não ser uma boa escolha para casos que precisam parecer mais realistas. Exemplos de variáveis a testar:

- **Dificuldade de deslocamento conforme o tipo de terreno** (asfalto, pedregulho, barro, areia, ...)
- **Nível de segurança da região**, conforme o número de ocorrências policiais
- **Dificuldade de deslocamento conforme a intensidade do tráfego**

Os slides dizem que esses são "alguns exemplos", ou seja, outras variáveis também podem ser inseridas na heurística.

---

## 3. Implementação da busca (slide 35)

As estratégias de busca precisam de uma estrutura de dados para controlar a árvore de busca. Cada **nó** (estado) é uma abstração do ambiente, com 4 componentes:

| Componente | Significado |
|---|---|
| **Estado** | Informações que definem o nó atual |
| **Nó pai** | Indicação do nó antecessor |
| **Ação** | Funcionalidade que levou ao estado do nó atual |
| **Custo do caminho g(n)** | Custo desde a raiz |

---

## 4. Base teórica da aula

### 4.1 Objetivos da aula

- Experimentar o pathfinding com A\* em **2 cenários**: viagem de Natal à praia de Pipa e Lago Congelado.
- Aplicar o A\* no cenário do Lago Congelado no **Gymnasium**.

### 4.2 Função de avaliação

```
f(n) = g(n) + h(n)
```

- **g(n):** custo real do caminho desde o nó inicial até n.
- **h(n):** estimativa heurística do custo de n até o objetivo.
- A expansão é feita sempre com o nó de **menor f(n)** na fronteira (nós abertos). Se houver empate, pode-se expandir qualquer um dos empatados.

### 4.3 Propriedades do A\*

| Propriedade | Resultado |
|---|---|
| Completude | Sim, a não ser que exista uma quantidade infinita de nós com f ≤ f(nó objetivo) |
| Otimização de custo | Sim, **desde que a heurística seja admissível** (f(n) nunca superestima o custo da melhor solução a partir de n) |
| Complexidade de tempo | O(b^m), mas uma boa heurística pode reduzi-la substancialmente |
| Complexidade de espaço | O(b^m), pois mantém todos os nós na memória |

Onde **b** é o fator de ramificação (máximo de sucessores de qualquer nó) e **m** é o número máximo de ações de qualquer caminho (pode ser ∞).

### 4.4 Heurística admissível

- Uma heurística h(n) é admissível se, para todo nó n, **h(n) ≤ h\*(n)**, onde h\*(n) é o custo real de chegar ao objetivo a partir de n.
- Ela permite ao A\* **ignorar nós que não podem levar a uma solução mais barata** do que a já encontrada.
- Exemplo: a distância em linha reta nunca superestima a distância real por outro meio, como uma estrada.

### 4.5 Cenário 1 — Viagem de Natal a Pipa (exemplo de aula)

- Usa apenas distância para escolher a rota; h(n) é a **distância Euclidiana** (linha reta até Pipa).
- Exemplo: linha reta Natal–Pipa = 45 km; distância real de carro = 83 km.
- g(n) = distância real entre cidades vizinhas conectadas (17 trechos de estrada, em km).
- h(n) = linha reta de cada cidade até Pipa (ex.: Natal 45, Parnamirim 41, São José de Mipibu 27, Goianinha 18, Tibau do Sul 6, Pipa 0).
- A busca nos slides 6 a 14 termina em **Pipa com custo 83**. Seguindo os valores dos slides, o caminho é Natal → Parnamirim → São José de Mipibu → Goianinha → Tibau do Sul → Pipa (13 + 19 + 23 + 19 + 9 = 83 km). Esse trajeto foi reconstruído a partir dos valores de g(n), pois os slides não o escrevem de forma explícita.

### 4.6 Cenário 2 — Lago Congelado (exemplo de aula)

- Grid 4x4, com a **distância Manhattan** como base de h(n).
- Custo de uma célula para a adjacente = **1**.
- Agente em **(0,0)**, objetivo em **(3,3)**: h inicial = 6.
- Nos slides 18 a 32, a busca é executada passo a passo, com as listas de nós **fechados** e **abertos**, e termina com **g = 6** em (3,3).

---

## 5. Estrutura sugerida para o relatório

> **Atenção:** esta estrutura é uma sugestão minha para organizar a análise crítica. Os slides não detalham as seções. **Prevalece o documento-modelo do SIGAA.**

1. **Identificação** (disciplina, aluno, data)
2. **Link de acesso ao código** (notebook adaptado, com permissão de acesso aberta)
3. **Descrição do ambiente e da implementação** (estrutura do nó: estado, pai, ação, g(n); função heurística adotada)
4. **Experimentos de cenário**
   - Tamanhos de grid testados
   - Posições de início, objetivo e buracos
5. **Experimentos de custo**
   - Variáveis de custo inseridas (terreno, segurança, tráfego ou outras)
   - Como cada variável entrou em g(n) e/ou h(n)
6. **Resultados** (caminhos encontrados, custo total, nós expandidos, comparação entre configurações)
7. **Análise crítica**
   - A heurística continuou admissível depois das variáveis de custo?
   - O efeito de cada variação sobre a rota e o desempenho do A\*
   - Limitações e pontos de melhoria
8. **Conclusão**

---

## 6. Checklist antes de entregar

- [ ] Relatório redigido no **formato do documento do SIGAA**
- [ ] **Link do código** incluído no relatório e **aberto** (testar em janela anônima)
- [ ] Variações de **cenário** testadas (tamanho, início, objetivo, buracos)
- [ ] Variáveis de **custo** testadas (terreno, segurança, tráfego ou outras)
- [ ] **Análise crítica** presente, e não só a descrição dos resultados
- [ ] Postado na atividade do **SIGAA**