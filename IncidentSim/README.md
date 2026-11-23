### IncidentSim

<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 5px;">
 <img align="center" alt="Python" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/python/python-original.svg" />
 <img align="center" alt="Jupyter" height="60" width="40" src="https://cdn.jsdelivr.net/gh/devicons/devicon@latest/icons/jupyter/jupyter-original.svg" />
</div>

### Sobre o projeto

Proposta de projeto final de [DIM0126 - Inteligência Artificial para Jogos I](../README.md), feito em dupla. **Situação: ideia, aguardando a opinião do professor.** Ainda não há código.

A ideia é um agente "de plantão" para uma plataforma de dados simulada. O simulador sorteia falhas, o agente vê só os sintomas e precisa escolher a ação certa para recuperar o sistema com o menor prejuízo. O projeto compara três técnicas da disciplina nesse mesmo ambiente: árvore de comportamento, A* e Q-learning.

### Resumo para o professor

> Professor, queria sua opinião sobre uma ideia de projeto final. É um simulador em Python, no estilo do FrozenLake, de uma plataforma de dados que sofre falhas sorteadas, como uma API fora do ar ou dados sujos chegando. Um agente "de plantão" precisa descobrir a causa só pelos sintomas e escolher a ação certa para recuperar o sistema com o menor prejuízo. A ideia é comparar três abordagens da disciplina: uma árvore de comportamento escrita à mão, o Q-learning e o A\*. O A\* conhece a causa real e serve de referência para a solução ótima.
>
> Por exemplo: no minuto 3 chega um lote de dados sujos, e o agente só vê que o Silver ficou degradado. A árvore de comportamento reinicia o Silver, o que não resolve, depois põe o lote em quarentena e reprocessa, recuperando no minuto 7. O Q-learning, se tiver aprendido a reconhecer a pista (muitos erros de validação), vai direto para a quarentena e o reprocessamento e recupera no minuto 6. O A\*, que sabe a causa real, mostra que 6 é o melhor tempo possível.
>
> Como não é um jogo tradicional, queria saber se o senhor acha que se encaixa na proposta da disciplina.

### O ambiente

Uma cadeia de 6 componentes, parecida com a de uma plataforma de dados real:

```
API externa → gateway → ingestão → Bronze → Silver → Gold
```

- **API externa:** o sistema de outra empresa, de onde vêm os dados
- **Gateway:** a porta de entrada, o serviço que recebe esses dados
- **Ingestão:** o processo que pega os dados e guarda
- **Bronze → Silver → Gold:** etapas de limpeza. O Bronze tem o dado bruto, o Silver o dado limpo e o Gold o dado pronto para relatórios e painéis

Cada componente tem só 3 estados (ok, degradado ou fora), e cada passo da simulação é 1 "minuto". O ambiente segue a interface do Gymnasium, igual ao FrozenLake. Não precisa subir Spark, Airflow nem Docker.

Não existe sistema de verdade rodando, é tudo simulado. Funciona como os buracos do FrozenLake: lá o gerador sorteia *onde* ficam os buracos, aqui o simulador sorteia *quando* e *qual* incidente acontece. A cada minuto em que está tudo ok, há uns 5% de chance de começar um incidente. A semente é fixa, então todas as políticas enfrentam exatamente os mesmos incidentes nos mesmos minutos, e a comparação fica justa.

### Incidentes

| Incidente | Sintoma que o agente vê | Conserto certo |
|-----------|-------------------------|----------------|
| API externa caiu | Gateway fora | **Esperar**: ela volta sozinha em 5–10 min |
| Deploy ruim no gateway | Gateway fora | **Rollback** |
| Lote ruim na ingestão (nulos, placeholders) | Silver degradado | **Quarentena** e depois **reprocessar**. Se reprocessar antes, o dado sujo chega no Gold |
| Job do Silver falhou | Silver degradado | **Reiniciar** |

Enquanto algo está quebrado, o Gold fica desatualizado e o prejuízo sobe a cada minuto.

**Observação parcial:** o agente vê o sintoma, não a causa. Cada sintoma tem duas causas possíveis com consertos diferentes, e o agente precisa deduzir qual é a certa pelas pistas (por exemplo, uma taxa alta de erros de validação indica lote ruim). A percepção do agente funciona como um painel de monitoramento (taxa de erro, latência, status).

### Ações e recompensa

Ações: **esperar**, **reiniciar**, **rollback**, **quarentena**, **reprocessar**.

Recompensa:
- cada minuto com o Gold desatualizado custa **-1**
- cada ação tem um custo próprio
- dado ruim chegar no Gold gera uma **penalidade grande**

### Políticas comparadas

| Política | Papel |
|----------|-------|
| Aleatória | Base de comparação, o pior caso |
| Árvore de comportamento | O "runbook" escrito à mão, como o que um time de backend escreve para quando o sistema cai |
| Q-learning | O agente aprende sozinho, a partir dos sintomas e das pistas |
| A* | O gabarito: conhece a causa real e busca a sequência ótima de ações no espaço de estados. Não é uma política realizável, e sim um limite inferior de prejuízo com informação perfeita |

**Pergunta do trabalho:** o runbook escrito à mão e o agente que aprende chegam perto da recuperação ótima?

**Métricas:** tempo até recuperar, minutos de Gold desatualizado, dado ruim que chegou no Gold e custo das ações, comparando as médias em milhares de episódios.

### Exemplo de funcionamento

No minuto 3 chega um lote ruim na ingestão.

| Minuto | Runbook (árvore) | Q-learning | A* (sabe a causa) |
|--------|------------------|------------|-------------------|
| 4 | Silver degradado → reinicia o Silver ❌ | Quarentena do lote | Quarentena do lote |
| 5 | Quarentena do lote | Reprocessa o Silver | Reprocessa o Silver |
| 6 | Reprocessa o Silver | Gold atualizado ✅ | Gold atualizado ✅ |
| 7 | Gold atualizado ✅ | | |

Nesse caso, o Q-learning chegou ao ótimo e o runbook perdeu um minuto por errar o diagnóstico.

### Relação com a disciplina

| Conteúdo | Onde aparece no projeto | Atividade relacionada |
|----------|-------------------------|-----------------------|
| Arquitetura de agente (percepção → decisão → ação) | Painel de monitoramento → escolha da ação → conserto | [FPSAgentDesign](../FPSAgentDesign) |
| Autoria de comportamento | O runbook como árvore de comportamento | [BehaviorAuthoring](../BehaviorAuthoring) |
| Busca / A* | A* no espaço de estados: cada nó é uma situação do sistema, cada aresta uma ação e o custo é o prejuízo | [RomaniaAStar](../RomaniaAStar), [FrozenLake](../FrozenLake) |
| Aprendizado por reforço | Q-learning num ambiente Gymnasium | [QLearningToyText](../QLearningToyText) |
| Comparação de técnicas de decisão | Árvore × busca ótima × aprendizado × aleatório | [TicTacToeAI](../TicTacToeAI) |
| Ambiente com sorteio e semente fixa | Incidentes sorteados como os buracos do lago | [FrozenLake](../FrozenLake) |

### Perguntas para o professor

1. Um ambiente que não é um jogo tradicional vale, desde que use as técnicas da disciplina? Ou é melhor embalar como um jogo (por exemplo, um jogo de gerenciamento)?
2. A comparação entre árvore de comportamento, A* e Q-learning é profunda o bastante para um projeto final, ou ele espera outra técnica (MCTS, algoritmo genético)?

### Pontos em aberto

- **Pistas do lado do gateway:** falta definir o que separa "API caiu" de "deploy ruim" (por exemplo, uma flag de deploy recente, ou o tipo de erro: timeout/502 vindo de fora contra 500 do próprio gateway). Sem isso, o Q-learning não tem como diferenciar as duas causas.
- **Tamanho do estado do Q-learning:** 6 componentes × 3 estados mais as pistas já dá centenas de estados. As pistas devem ser discretizadas em poucos valores (por exemplo, taxa de erro alta ou baixa).
- **A\* determinístico:** o A* só funciona porque, com a semente fixa e a causa conhecida, o futuro é determinístico (inclusive o minuto em que a API volta).

### Planejamento (4 semanas)

1. Ambiente pronto, com a política aleatória rodando
2. Árvore de comportamento e A*, com os primeiros resultados
3. Q-learning e os experimentos (vários cenários e sementes)
4. Relatório e apresentação, com uma linha do tempo de cada incidente mostrando o que cada política fez

Divisão:
- **Lado dos serviços** (API externa, gateway, deploy) e **árvore de comportamento**: integração com API externa, backend, CI/CD e observabilidade (Grafana, Prometheus, Loki)
- **Lado dos dados** (ingestão, Bronze/Silver/Gold, lote ruim, job falhando) e **métricas e análise dos experimentos**
- **Juntos:** A*, Q-learning e relatório

O simulador deve ficar com umas 150 a 200 linhas de Python.

**Extras, se sobrar tempo:** cache no gateway, incidentes que se propagam entre as camadas.

### Alternativas consideradas

1. Otimizador de ordem de joins em SQL com A*, algoritmo genético e MCTS
2. Agente que "joga" contra uma API REST para achar bugs
