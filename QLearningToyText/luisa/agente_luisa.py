"""Agente Q-learning com taxa de aprendizado decrescente (FrozenLake).

Fica num módulo separado do notebook para que as sementes possam rodar em paralelo
(no Windows, o multiprocessing só consegue chamar funções importáveis de um módulo).
"""
from dataclasses import dataclass

import gymnasium as gym
import numpy as np
from concurrent.futures import ProcessPoolExecutor


@dataclass(frozen=True)
class Config:
    alpha: float = 0.5             # taxa de aprendizado (constante, ou inicial no modo "episodio")
    gamma: float = 0.95            # fator de desconto
    epsilon_inicial: float = 0.5
    epsilon_final: float = 0.1
    episodios: int = 5000
    freq_teste: int = 25
    episodios_teste: int = 20
    episodios_teste_final: int = 500
    # --- novos em relação ao notebook do Caio: como α muda ao longo do treino ---
    alpha_modo: str = "const"      # "const", "visitas" ou "episodio"
    omega: float = 1.0             # "visitas": α = max(alpha_min, 1 / (1 + N(s,a))^omega)
    alpha_min: float = 0.0
    alpha_final: float = 0.01      # "episodio": α cai linearmente de alpha até alpha_final


def frozen_lake(desc, escorregadio):
    return gym.make("FrozenLake-v1", desc=list(desc), is_slippery=escorregadio)


def q_otimo(desc, escorregadio, gamma, iteracoes=2000):
    """Q* do problema descontado (o ponto fixo para o qual o Q-learning converge), por iteração de valor."""
    P = frozen_lake(desc, escorregadio).unwrapped.P
    n_s, n_a = len(P), len(P[0])
    plano = "".join(desc)
    Q = np.zeros((n_s, n_a))
    for _ in range(iteracoes):
        V = Q.max(1)
        Q2 = np.zeros_like(Q)
        for s in range(n_s):
            if plano[s] in "HG":
                continue
            for a in range(n_a):
                Q2[s, a] = sum(p * (r + (0 if d else gamma * V[s2])) for p, s2, r, d in P[s][a])
        if np.abs(Q2 - Q).max() < 1e-10:
            return Q2
        Q = Q2
    return Q


class QLearningAgent:
    def __init__(self, n_estados, n_acoes, cfg: Config, seed: int):
        self.cfg = cfg
        self.q_table = np.zeros((n_estados, n_acoes))
        self.visitas = np.zeros((n_estados, n_acoes))
        self.epsilon = cfg.epsilon_inicial
        self.epsilon_decay = (cfg.epsilon_inicial - cfg.epsilon_final) / cfg.episodios
        self.alpha_ep = cfg.alpha                      # usado nos modos "const" e "episodio"
        self.alpha_decay = (cfg.alpha - cfg.alpha_final) / cfg.episodios
        self.rng = np.random.default_rng(seed)

    def eval_state(self, state):
        # ação gulosa; empates são desfeitos ao acaso
        q = self.q_table[state]
        melhores = np.flatnonzero(q == q.max())
        if len(melhores) == 1:
            return int(melhores[0])
        return int(melhores[self.rng.integers(len(melhores))])

    def choose_action(self, state):
        if self.rng.uniform() < self.epsilon:
            return int(self.rng.integers(self.q_table.shape[1]))
        return self.eval_state(state)

    def taxa(self, state, action):
        if self.cfg.alpha_modo == "visitas":
            n = self.visitas[state, action]
            return max(self.cfg.alpha_min, 1.0 / (1.0 + n) ** self.cfg.omega)
        return self.alpha_ep

    def update(self, state, action, reward, terminated, next_state):
        if terminated:
            target = reward
        else:
            target = reward + self.cfg.gamma * np.max(self.q_table[next_state])
        td_error = target - self.q_table[state, action]
        alpha = self.taxa(state, action)
        self.visitas[state, action] += 1
        self.q_table[state, action] += alpha * td_error
        return td_error, alpha

    def fim_episodio(self):
        self.epsilon = max(self.cfg.epsilon_final, self.epsilon - self.epsilon_decay)
        if self.cfg.alpha_modo == "episodio":
            self.alpha_ep = max(self.cfg.alpha_final, self.alpha_ep - self.alpha_decay)


def avaliar(env, agente, n_episodios):
    total = 0.0
    for _ in range(n_episodios):
        state, _ = env.reset()
        done = False
        while not done:
            state, reward, terminated, truncated, _ = env.step(agente.eval_state(state))
            total += reward
            done = terminated or truncated
    return total / n_episodios


def treinar(desc, escorregadio, cfg: Config, seed: int, q_star=None):
    """Treina um agente. Além das métricas do notebook do Caio, registra a cada teste:
    o erro médio |Q - Q*|, o valor estimado do estado inicial, quantos estados mudaram de ação gulosa
    desde o teste anterior e o α médio usado no período."""
    env = frozen_lake(desc, escorregadio)
    env.reset(seed=seed)
    agente = QLearningAgent(env.observation_space.n, env.action_space.n, cfg, seed)
    plano = np.array(list("".join(desc)))
    livres = ~np.isin(plano, ["H", "G"])               # estados não terminais

    retornos, passos, aval, erro_q, v0, trocas, alpha_medio = [], [], [], [], [], [], []
    politica_ant = agente.q_table.argmax(1)
    soma_alpha, n_alpha = 0.0, 0

    for episodio in range(cfg.episodios):
        state, _ = env.reset()
        done, total, n = False, 0.0, 0
        while not done:
            action = agente.choose_action(state)
            next_state, reward, terminated, truncated, _ = env.step(action)
            _, a = agente.update(state, action, reward, terminated, next_state)
            soma_alpha += a
            n_alpha += 1
            state = next_state
            total += reward
            n += 1
            done = terminated or truncated
        agente.fim_episodio()
        retornos.append(total)
        passos.append(n)
        if (episodio + 1) % cfg.freq_teste == 0:
            aval.append(avaliar(env, agente, cfg.episodios_teste))
            q = agente.q_table
            if q_star is not None:
                erro_q.append(np.abs(q[livres] - q_star[livres]).mean())
            v0.append(q[0].max())
            politica = q.argmax(1)
            trocas.append(int((politica[livres] != politica_ant[livres]).sum()))
            politica_ant = politica
            alpha_medio.append(soma_alpha / max(n_alpha, 1))
            soma_alpha, n_alpha = 0.0, 0

    final = avaliar(env, agente, cfg.episodios_teste_final)
    env.close()
    return {"retornos": np.array(retornos), "passos": np.array(passos), "aval": np.array(aval),
            "erro_q": np.array(erro_q), "v0": np.array(v0), "trocas": np.array(trocas),
            "alpha_medio": np.array(alpha_medio), "final": final, "q_table": agente.q_table,
            "visitas": agente.visitas, "cfg": cfg}


def _rodar(tarefa):
    chave, desc, escorregadio, cfg, seed, q_star = tarefa
    r = treinar(desc, escorregadio, cfg, seed, q_star)
    return chave, seed, r


def rodar_lote(tarefas, processos=None):
    """Roda em paralelo uma lista de (chave, desc, escorregadio, cfg, seed, q_star).
    Devolve {chave: [resultado por semente, na ordem das sementes]}."""
    saida = {}
    with ProcessPoolExecutor(max_workers=processos) as pool:
        for chave, seed, r in pool.map(_rodar, tarefas):
            saida.setdefault(chave, []).append((seed, r))
    return {k: [r for _, r in sorted(v, key=lambda x: x[0])] for k, v in saida.items()}
