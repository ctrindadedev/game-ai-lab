### QLearningToyText

<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 5px;">
 <img align="center" alt="Python" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/python/python-original.svg" />
 <img align="center" alt="Jupyter" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/jupyter/jupyter-original.svg" />
</div>

### Sobre o projeto

Atividade 5 de [DIM0126 - Inteligência Artificial para Jogos I](../README.md). Experimentos com o Q-learning tabular nos ambientes Toy Text do [Gymnasium](https://gymnasium.farama.org/environments/toy_text/), a partir do notebook do Lago Congelado disponibilizado na disciplina.

Todo o código e os resultados estão em [`qlearning_toy_text.ipynb`](./qlearning_toy_text.ipynb), já executado. O agente é o mesmo do notebook original (Q-table e ε-greedy com decaimento linear), com quatro mudanças:

- usa só o `gymnasium` (o original importava partes do pacote antigo `gym`) e não grava vídeos;
- o mapa 8x8 é gerado como no original (`generate_random_map(size=8, p=0.9)`), mas com semente fixa, para comparar todas as configurações no mesmo mapa;
- cada configuração roda com 5 sementes, e os resultados são média ± desvio padrão;
- a política gulosa é testada a cada 25 episódios (no original, a cada 100), para que a velocidade de aprendizado apareça nas curvas.

### Enunciado

- Experimentar o método Q-learning com o ambiente do Lago Congelado (Frozen Lake) do Toy Text do Gymnasium, usando o notebook do Colab disponibilizado
- Testar o Q-learning variando os hiperparâmetros (taxas de aprendizado, exploração, ...)
- Testar o Q-learning em outros cenários do Toy Text

### Experimentos

| # | O que varia | Ambiente |
|---|-------------|----------|
| 0 | nada (configuração do notebook original: α = 0.5, γ = 0.95, ε de 0.5 a 0.1, 5000 episódios) | FrozenLake 8x8 determinístico |
| 1 | o mapa: 10 mapas aleatórios contra o mapa fixo | FrozenLake 8x8 determinístico |
| 2 | α, γ, ε inicial e ε final, um de cada vez | FrozenLake 8x8 determinístico e 4x4 escorregadio |
| 3 | tamanho do mapa (4x4 e 8x8), `is_slippery` e treino mais longo no 8x8 escorregadio | FrozenLake |
| 4 | o ambiente | CliffWalking-v1 e Taxi-v4 |

Os gráficos ficam em `figuras/`. Ao rodar, o notebook também gera a tabela geral em `resultados.csv` (não versionada).

### Rodando

Precisa de Python 3.10+.

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
.venv/bin/jupyter notebook qlearning_toy_text.ipynb
```

Para reexecutar tudo sem abrir o Jupyter (leva alguns minutos):

```sh
.venv/bin/jupyter nbconvert --to notebook --execute --inplace qlearning_toy_text.ipynb
```

O notebook também roda no Colab: ele instala o `gymnasium` se não estiver disponível.
