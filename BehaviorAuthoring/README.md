### BehaviorAuthoring

<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 5px;">
 <img align="center" alt="LaTeX" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/latex/latex-original.svg" />
</div>

### Sobre o projeto

Atividade 3 de [DIM0126 - Inteligência Artificial para Jogos I](../README.md), sobre autoria de comportamento: o game designer escreve explicitamente as regras que o personagem segue. São dois exercícios de modelagem:

- **PacMan como NPC**: ações, condições e a árvore de comportamento (BT) que as organiza.
- **Enderman (Minecraft)**: o mesmo comportamento modelado como BT e como máquina de estados hierárquica (HFSM), e uma análise de quando usar cada uma e de como combiná-las.

O entregável é o `autoria_comportamento.tex`.

### Enunciado

> **Exercício 1**
>
> 1. Se o PacMan fosse um NPC, como poderia ser sua Árvore de Comportamento?
>    1. Defina primeiro as ações
>    2. Defina as condições
>    3. Tente organizar usando as composições
>
> **Exercício 2**
>
> 1. Escolha um personagem de um jogo de seu interesse e:
>    1. Crie árvore de comportamento que se assemelhe a seu comportamento
>    2. Defina o mesmo comportamento através de uma máquina de estados (pode ser hierárquica)
> 2. Baseado nessa experiência, indique
>    1. Quais situações é mais interessante modelar um personagem através de uma máquina de estados
>    2. Quais situações é mais interessante modelar um personagem através de uma árvore de comportamento
>    3. É possível combinar os "dois mundos"? Vale a pena? Se sim, quando isso é interessante?

### Compilando

```sh
pdflatex autoria_comportamento.tex
```
