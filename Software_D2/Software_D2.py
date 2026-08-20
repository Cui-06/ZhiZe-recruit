# -*- coding: utf-8 -*-
import os
import sys

import matplotlib.pyplot as plt
import numpy as np


if os.name == "nt":
    os.system("chcp 65001 > nul")
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8")

np.random.seed(0)  # 做题先写 “解”

RESULT_DIR = "results"
TRIAL = 30000  # 训练 3 万次
EXPLORE_STOP = 20000  # 在 2 万次之后取消掉随机性


# 定义井字棋智能体，用于自我对弈训练。
class Agent():

    def __init__(self, OOXX_Index, Epsilon, Alpha):
        self.index = OOXX_Index  # OOXX_Index 用 1 或者 2 代表是两个 Agent 当中的哪一个
        self.epsilon = Epsilon  # Epsilon 就是 ε-Greedy 策略中的随机选择概率
        self.alpha = Alpha  # Alpha 就是学习率
        self.value = np.zeros((3, 3, 3, 3, 3, 3, 3, 3, 3))  # 储存状态价值的表
        # OOXX 的棋盘一共有 9 个位置，每个位置有 3 种情况（O、X、无）
        # 因此用 9 维数组表示状态，并用 3^9 的表储存状态价值。
        self.stored_Outcome = np.zeros(9).astype(np.int8)  # Agent 内部记录的后果，初始化为 0，表示空棋盘

    def reset(self):
        self.stored_Outcome = np.zeros(9).astype(np.int8)

    # 输入当前状态，输出落子后的状态，并按 TD 误差更新价值表。
    def move(self, State, learn=True):
        Outcome = State.copy()  # 拷贝一份状态
        available = np.where(Outcome == 0)[0]  # 先判断棋盘上有哪些地方可以落子，也就是 Outcome==0 的地方
        if np.random.binomial(1, self.epsilon):  # 判断要不要采取 ε-Greedy 的随机行动
            Outcome[np.random.choice(available)] = self.index  # 随机选择一个位置标注为 1 或着 2 （取决于是 Agent1 还是 Agent2）
        else:  # 如果不随机，就采用最优策略
            temp_Value = np.zeros(len(available))  # 创建一个临时的价值向量
            for i in range(len(available)):  # 对每一个可能落子的地方
                temp_Outcome = Outcome.copy()  # 拷贝当前时刻的状态
                temp_Outcome[available[i]] = self.index  # 假设在一个地方落子，得到后果
                temp_Value[i] = self.value[tuple(temp_Outcome)]  # 调用价值函数，计算得到的后果的价值
            choose = np.argmax(temp_Value)  # 选择价值最大的那一个行动
            Outcome[available[choose]] = self.index  # 把选择的那个位置标注为 1 或着 2 （取决于是 Agent1 还是 Agent2）

        if learn:
            # 基于误差的学习法，也就是时序差分法。
            Error = self.value[tuple(Outcome)] - self.value[tuple(self.stored_Outcome)]
            self.value[tuple(self.stored_Outcome)] += self.alpha * Error  # 更新储存的（上一个）后果的价值估计
            self.stored_Outcome = Outcome.copy()

        return Outcome  # 返回当前的后果


# 写一个函数判断输赢
def Judge(Outcome, OOXX_Index):  # 输入为状态和对应的玩家
    Triple = np.repeat(OOXX_Index, 3)
    winner = 0  # 默认胜负未分
    if 0 not in Outcome:  # 没地方下了
        winner = 3  # 平局
    if (Outcome[0:3] == Triple).all() or (Outcome[3:6] == Triple).all() or (Outcome[6:9] == Triple).all():  # 分别判断三行
        winner = OOXX_Index
    if (Outcome[0:7:3] == Triple).all() or (Outcome[1:8:3] == Triple).all() or (Outcome[2:9:3] == Triple).all():  # 分别判断三列
        winner = OOXX_Index
    if (Outcome[0:9:4] == Triple).all() or (Outcome[2:7:2] == Triple).all():  # 分别判断两条对角线
        winner = OOXX_Index
    return winner  # 返回玩家是否胜利


def train():
    # 创建两个 Agent
    Agent1 = Agent(OOXX_Index=1, Epsilon=0.1, Alpha=0.1)
    Agent2 = Agent(OOXX_Index=2, Epsilon=0.1, Alpha=0.1)

    Winner = np.zeros(TRIAL)  # 记录结果

    for i in range(TRIAL):
        if i == EXPLORE_STOP:  # 在 2 万次之后取消掉随机性
            Agent1.epsilon = 0
            Agent2.epsilon = 0
        Agent1.reset()  # 重置状态
        Agent2.reset()  # 重置状态
        winner = 0  # 默认胜负未分
        State = np.zeros(9).astype(np.int8)  # 初始化棋盘
        # 默认 Agent1 先行。
        while winner == 0:  # 如果胜负未分
            Outcome = Agent1.move(State, learn=True)  # Agent1 采取行动，并且更新价值
            winner = Judge(Outcome, 1)  # 判断 Agent1 是否获胜
            if winner == 1:  # 如果 Agent1 获胜
                Agent1.value[tuple(Outcome)] = 1  # Outcome 的价值对 Agent1 来说为 1
                Agent2.value[tuple(State)] = -1  # Agent2 对应的后果，也就是 Agent1 面临的 State 的价值对 Agent2 来说为 -1
            elif winner == 0:  # 如果胜负未分
                State = Agent2.move(Outcome, learn=True)  # Agent2 采取行动，并且更新价值
                winner = Judge(State, 2)  # 判断 Agent2 是否获胜
                if winner == 2:  # 如果 Agent2 获胜
                    Agent2.value[tuple(State)] = 1  # Agent2 对应的后果，也就是 Agent1 面临的 State 的价值对 Agent2 来说为 1
                    Agent1.value[tuple(Outcome)] = -1  # Outcome 的价值对 Agent1 来说为 -1
        Winner[i] = winner  # 记录结果

    return Agent1, Agent2, Winner


# 根据结果计算胜率
step = 250  # 每隔250局游戏计算一次胜率
duration = 500  # 胜率根据前后共500局来计算
def Rate(Winner):
    Rate1 = np.zeros(int((TRIAL - duration) / step) + 1)  # Agent1 胜率
    Rate2 = np.zeros(int((TRIAL - duration) / step) + 1)  # Agent2 胜率
    Rate3 = np.zeros(int((TRIAL - duration) / step) + 1)  # 平局概率
    for i in range(len(Rate1)):
        Rate1[i] = np.sum(Winner[step * i:duration + step * i] == 1) / duration
        Rate2[i] = np.sum(Winner[step * i:duration + step * i] == 2) / duration
        Rate3[i] = np.sum(Winner[step * i:duration + step * i] == 3) / duration
    return Rate1, Rate2, Rate3


def save_results(Agent1, Agent2, Winner):
    os.makedirs(RESULT_DIR, exist_ok=True)

    Rate1, Rate2, Rate3 = Rate(Winner)

    np.save(os.path.join(RESULT_DIR, "agent1_value.npy"), Agent1.value)
    np.save(os.path.join(RESULT_DIR, "agent2_value.npy"), Agent2.value)
    np.savetxt(os.path.join(RESULT_DIR, "winners.csv"), Winner, fmt="%d", delimiter=",")

    fig, ax = plt.subplots(figsize=(16, 9))
    plt.plot(Rate1, linewidth=4, marker='.', markersize=20, color="#0071B7", label="Agent1")
    plt.plot(Rate2, linewidth=4, marker='.', markersize=20, color="#DB2C2C", label="Agent2")
    plt.plot(Rate3, linewidth=4, marker='.', markersize=20, color="#FAB70D", label="Draw")
    plt.xticks(np.arange(0, 121, 40), np.arange(0, 31 + 1, 10), fontsize=30)
    plt.yticks(np.arange(0, 1.1, 0.2), np.round(np.arange(0, 1.1, 0.2), 2), fontsize=30)
    plt.xlabel("Trials(x1k)", fontsize=30)
    plt.ylabel("Winning Rate", fontsize=30)
    plt.legend(loc="best", fontsize=25)
    plt.tick_params(width=4, length=10)
    ax.spines[:].set_linewidth(4)
    plt.tight_layout()
    plt.savefig(os.path.join(RESULT_DIR, "training_curve.png"), dpi=150)
    plt.close()

    return Rate1, Rate2, Rate3


def print_board(State):
    symbol = {0: " ", 1: "O", 2: "X"}
    board = [symbol[int(x)] for x in State]
    print(f" {board[0]} | {board[1]} | {board[2]} ")
    print("---+---+---")
    print(f" {board[3]} | {board[4]} | {board[5]} ")
    print("---+---+---")
    print(f" {board[6]} | {board[7]} | {board[8]} ")


def demo_game(Agent1, Agent2):
    old_epsilon1 = Agent1.epsilon
    old_epsilon2 = Agent2.epsilon
    Agent1.epsilon = 0
    Agent2.epsilon = 0

    State = np.zeros(9).astype(np.int8)
    winner = 0
    step_idx = 1

    print("\n最终策略演示：Agent1(O) vs Agent2(X)")
    print_board(State)

    while winner == 0:
        Outcome = Agent1.move(State, learn=False)
        winner = Judge(Outcome, 1)
        print(f"\n第 {step_idx} 步：Agent1(O)")
        print_board(Outcome)
        step_idx += 1
        if winner != 0:
            break

        State = Agent2.move(Outcome, learn=False)
        winner = Judge(State, 2)
        print(f"\n第 {step_idx} 步：Agent2(X)")
        print_board(State)
        step_idx += 1

    if winner == 1:
        print("\n结果：Agent1 获胜")
    elif winner == 2:
        print("\n结果：Agent2 获胜")
    else:
        print("\n结果：平局")

    Agent1.epsilon = old_epsilon1
    Agent2.epsilon = old_epsilon2


def main():
    Agent1, Agent2, Winner = train()
    Rate1, Rate2, Rate3 = save_results(Agent1, Agent2, Winner)

    print("训练完成")
    print(f"Agent1 最近胜率：{Rate1[-1]:.3f}")
    print(f"Agent2 最近胜率：{Rate2[-1]:.3f}")
    print(f"最近平局概率：{Rate3[-1]:.3f}")
    print(f"训练曲线已保存到：{os.path.join(RESULT_DIR, 'training_curve.png')}")
    print(f"价值表已保存到：{RESULT_DIR}/agent1_value.npy 和 {RESULT_DIR}/agent2_value.npy")

    demo_game(Agent1, Agent2)


if __name__ == "__main__":
    main()
