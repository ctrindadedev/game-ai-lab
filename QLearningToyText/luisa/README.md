### QLearningToyText / luisa

<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 5px;">
 <img align="center" alt="Python" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/python/python-original.svg" />
 <img align="center" alt="Jupyter" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/jupyter/jupyter-original.svg" />
</div>

### Sobre esta pasta

Versão **individual da Luisa** da Atividade 5 de [DIM0126 - Inteligência Artificial para Jogos I](../../README.md) (Q-learning nos ambientes Toy Text do Gymnasium).

O relatório desta atividade é individual, mas o código foi desenvolvido no mesmo repositório que o do colega Caio Trindade ([`../qlearning_toy_text.ipynb`](../qlearning_toy_text.ipynb)). Por isso esta pasta **parte de uma cópia do notebook dele** (mesmo agente da disciplina, mesma classe `Config`, mesmo protocolo de 5 sementes e teste guloso a cada 25 episódios) e **muda o que é estudado**, para que os dois trabalhos não tenham os mesmos experimentos:

| Tema | Notebook do Caio | Esta pasta |
|------|------------------|------------|
| Pergunta central | efeito de α, γ e ε (constantes) e de outros ambientes | **taxa de aprendizado α decrescente** no gelo escorregadio |
| α | constante: 0.01, 0.1, 0.5, 0.9 | constante (0.5, 0.1, 0.02) como referência, contra $1/(1+N(s,a))^\omega$ com ω = 1, 0.8 e 0.6, com piso de 0.05 e decaimento linear por episódio (0.5 → 0.01) |
| Mapas | 8x8 aleatório (`p=0.9`, semente 9) e 4x4 padrão | `FrozenLake8x8-v1` padrão e 8x8 aleatório com `p=0.8` (mais buracos) |
| Treino no escorregadio | 5000 episódios (20000 em um teste) | 20000 episódios em todas as configurações |
| Métricas | sucesso da política gulosa, recompensa de treino | sucesso, **erro da Q-table contra o Q\* exato** (iteração de valor), **trocas de ação gulosa entre testes** (estabilidade) e acordo com a política ótima |

γ = 0.95 e a exploração (ε de 0.5 a 0.1, linear) ficam iguais à configuração base do notebook original, para que só α mude.

### Arquivos

| Arquivo | Conteúdo |
|---------|----------|
| [`qlearning_toy_text_luisa.ipynb`](./qlearning_toy_text_luisa.ipynb) | notebook com os experimentos e todas as saídas, já executado |
| [`agente_luisa.py`](./agente_luisa.py) | agente Q-learning com as programações de α, cálculo de Q\* e o treino em paralelo |
| [`relatorio_qlearning_luisa.tex`](./relatorio_qlearning_luisa.tex) | relatório (Ambientes, Metodologia, Resultados, Discussão, Conclusões) |
| `figuras/` | gráficos gerados pelo notebook e usados no relatório |

O agente fica num módulo `.py` separado porque, no Windows, o `multiprocessing` só consegue executar funções importáveis de um módulo; assim as 120 execuções (8 programações de α × 3 ambientes × 5 sementes) rodam em paralelo. Cada execução usa apenas a própria semente, então o resultado não depende do número de processos.

### Experimentos

| # | O que varia | Ambiente |
|---|-------------|----------|
| 0 | forma das programações de α (só o gráfico de α em função de N) | — |
| 1 | as 8 programações de α (controle: sem ruído de transição) | FrozenLake 8x8 padrão determinístico, 5000 episódios |
| 2 | as 8 programações de α | FrozenLake 8x8 padrão escorregadio, 20000 episódios |
| 3 | as 8 programações de α (segundo mapa) | FrozenLake 8x8 `p=0.8` escorregadio, 20000 episódios |
| 4 | políticas aprendidas contra a ótima e visitas por estado | FrozenLake 8x8 padrão escorregadio |

### Principais resultados

Sucesso da política gulosa final em % do máximo possível (média de 5 sementes, 20000 episódios no gelo escorregadio):

| α | 8x8 padrão | 8x8 `p=0.8` |
|---|-----------:|------------:|
| 0.5 constante (base) | 34% | 42% |
| 0.1 constante | 80% | 75% |
| 1/(1+N) | 28% | 11% |
| **1/(1+N)^0.6** | **92%** | **92%** |
| linear 0.5 → 0.01 | 93% | 94% |

- No determinístico, todas as programações chegam a 100%, no mesmo episódio: sem ruído de transição, α não muda a política.
- No escorregadio, **decair α devagar e por par (ω = 0.6)** foi o melhor nos dois mapas, com o menor erro contra Q\* e a política estável no fim do treino.
- **1/(1+N), a média simples, foi pior que a base**: α cai tão rápido que a Q-table congela antes de convergir. A política para de mudar, mas está errada.
- O decaimento linear por episódio chega ao mesmo resultado final, mas oscila como α = 0.5 durante boa parte do treino.

A discussão completa está no fim do notebook e no relatório.

### Rodando

Precisa de Python 3.10+ e das mesmas dependências do notebook do Caio ([`../requirements.txt`](../requirements.txt)). A partir de `QLearningToyText/`:

```sh
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
cd luisa
../.venv/bin/jupyter notebook qlearning_toy_text_luisa.ipynb
```

Para reexecutar tudo sem abrir o Jupyter (cerca de 10 minutos com 12 núcleos):

```sh
../.venv/bin/jupyter nbconvert --to notebook --execute --inplace --ExecutePreprocessor.timeout=-1 qlearning_toy_text_luisa.ipynb
```

Ao rodar, o notebook também gera a tabela geral em `resultados_luisa.csv` (não versionada).
