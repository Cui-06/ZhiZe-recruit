import math

import numpy as np


def normalize(x):
    """把 [0, 2*pi] 的输入缩放到 [-1, 1]，训练会更稳定。"""
    return np.asarray(x, dtype=np.float64) / math.pi - 1.0


class SimpleBPNetwork:
    """单隐藏层 BP 神经网络：输入层 -> tanh 隐藏层 -> 输出层。"""

    def __init__(self, input_dim, hidden_dim, lr=0.003, seed=0):
        rng = np.random.default_rng(seed)
        self.lr = lr

        # Xavier 初始化，避免一开始的输出过大或过小。
        limit1 = np.sqrt(6.0 / (input_dim + hidden_dim))
        limit2 = np.sqrt(6.0 / (hidden_dim + 1))
        self.W1 = rng.uniform(-limit1, limit1, (input_dim, hidden_dim))
        self.b1 = np.zeros((1, hidden_dim))
        self.W2 = rng.uniform(-limit2, limit2, (hidden_dim, 1))
        self.b2 = np.zeros((1, 1))

        # Adam 优化器的一阶矩和二阶矩缓存，用于根据梯度自适应更新参数。
        self.m = [np.zeros_like(self.W1), np.zeros_like(self.b1),
                  np.zeros_like(self.W2), np.zeros_like(self.b2)]
        self.v = [np.zeros_like(self.W1), np.zeros_like(self.b1),
                  np.zeros_like(self.W2), np.zeros_like(self.b2)]
        self.step = 0

    def forward(self, X):
        # 前向传播：先计算隐藏层，再计算输出层。
        self.X = X
        self.Z1 = X @ self.W1 + self.b1
        # 隐藏层使用 tanh 激活函数，增强网络拟合非线性函数的能力。
        self.A1 = np.tanh(self.Z1)
        # 输出层不加激活函数，直接输出连续值，用于拟合 sin/cos 结果。
        self.Y = self.A1 @ self.W2 + self.b2
        return self.Y

    def train(self, X, Y, epochs=3000, batch_size=256):
        rng = np.random.default_rng(123)
        n = X.shape[0]

        for epoch in range(epochs):
            # 每轮训练前打乱样本顺序，避免固定顺序影响梯度更新。
            order = rng.permutation(n)
            X = X[order]
            Y = Y[order]

            # 使用 mini-batch 训练，兼顾训练速度和梯度稳定性。
            for start in range(0, n, batch_size):
                xb = X[start:start + batch_size]
                yb = Y[start:start + batch_size]

                # 计算当前 batch 的预测值。
                pred = self.forward(xb)
                batch_n = xb.shape[0]

                # MSE = mean((pred - y)^2)
                # 输出层梯度。
                dY = 2.0 * (pred - yb) / batch_n
                dW2 = self.A1.T @ dY
                db2 = np.sum(dY, axis=0, keepdims=True)

                # 隐藏层梯度，tanh'(x) = 1 - tanh(x)^2。
                dA1 = dY @ self.W2.T
                dZ1 = dA1 * (1.0 - self.A1 ** 2)
                dW1 = xb.T @ dZ1
                db1 = np.sum(dZ1, axis=0, keepdims=True)

                self.update([dW1, db1, dW2, db2])

    def update(self, grads):
        """Adam 参数更新，比普通梯度下降更容易训练到 1e-2 以内。"""
        beta1 = 0.9
        beta2 = 0.999
        eps = 1e-8
        params = [self.W1, self.b1, self.W2, self.b2]
        self.step += 1

        for i in range(len(params)):
            # m 是一阶矩估计，v 是二阶矩估计，用于自适应调整每个参数的步长。
            self.m[i] = beta1 * self.m[i] + (1.0 - beta1) * grads[i]
            self.v[i] = beta2 * self.v[i] + (1.0 - beta2) * (grads[i] ** 2)
            # 偏差修正，减少 Adam 在训练初期的估计偏差。
            m_hat = self.m[i] / (1.0 - beta1 ** self.step)
            v_hat = self.v[i] / (1.0 - beta2 ** self.step)
            params[i] -= self.lr * m_hat / (np.sqrt(v_hat) + eps)

    def predict(self, X):
        # 预测阶段只需要前向计算，不需要保存中间梯度。
        hidden = np.tanh(X @ self.W1 + self.b1)
        return hidden @ self.W2 + self.b2


# 一维输入的神经网络实现：拟合 y = sin(x)
class NeuralNetwork1D:
    def __init__(self):
        # 在 [0, 2*pi) 上均匀采样，生成一维训练数据。
        train_x = np.linspace(0, 2 * math.pi, 1024, endpoint=False).reshape(-1, 1)
        train_y = np.sin(train_x)

        # 建立并训练一维网络，用于拟合 y = sin(x)。
        self.net = SimpleBPNetwork(input_dim=1, hidden_dim=64, lr=0.003, seed=1)
        self.net.train(normalize(train_x), train_y, epochs=2500, batch_size=128)

    def predict(self, input_x: float) -> float:
        # 单个输入也要转换成二维数组，保持和训练时的数据形状一致。
        x = normalize([[input_x]])
        return float(self.net.predict(x)[0, 0])


# 二维输入的神经网络实现：拟合 y = sin(x1) * cos(x2)
class NeuralNetwork2D:
    def __init__(self):
        # 构造二维网格训练集，每个样本包含 x1 和 x2 两个输入。
        grid = np.linspace(0, 2 * math.pi, 50, endpoint=False)
        x1, x2 = np.meshgrid(grid, grid)
        train_x = np.column_stack([x1.ravel(), x2.ravel()])
        train_y = np.sin(train_x[:, 0:1]) * np.cos(train_x[:, 1:2])

        # 建立并训练二维网络，用于拟合 y = sin(x1) * cos(x2)。
        self.net = SimpleBPNetwork(input_dim=2, hidden_dim=192, lr=0.003, seed=2)
        self.net.train(normalize(train_x), train_y, epochs=7000, batch_size=256)

    def predict(self, input_x1: float, input_x2: float) -> float:
        # 将两个标量输入整理成一条二维输入样本。
        x = normalize([[input_x1, input_x2]])
        return float(self.net.predict(x)[0, 0])


class Test:
    def __init__(self):
        self.net1 = NeuralNetwork1D()
        self.net2 = NeuralNetwork2D()

    def output_y(self, *args) -> float:
        # 根据输入参数个数自动选择一维网络或二维网络。
        if len(args) == 1:
            return self.net1.predict(args[0])
        if len(args) == 2:
            return self.net2.predict(args[0], args[1])
        raise TypeError("output_y only supports 1D or 2D input")

    def testbench(self, num: int):
        sum_error = 0.0

        if num == 0:
            # 一维测试：比较网络输出和 math.sin(x) 的平均绝对误差。
            total = 500
            for i in range(total):
                x = 1.0 * i / total * 2 * math.pi
                y = self.output_y(x)
                sum_error += abs(math.sin(x) - y)
            average_error = sum_error / total
        else:
            # 二维测试：比较网络输出和 sin(x1) * cos(x2) 的平均绝对误差。
            total = 20
            for i in range(total):
                for j in range(total):
                    x1 = 1.0 * i / total * 2 * math.pi
                    x2 = 1.0 * j / total * 2 * math.pi
                    y = self.output_y(x1, x2)
                    true_y = math.sin(x1) * math.cos(x2)
                    sum_error += abs(true_y - y)
            average_error = sum_error / (total * total)

        label = "The 2D is " if num else "The 1D is "
        # 平均误差不超过 1e-2 时认为测试成功。
        if average_error <= 1e-2:
            print(f"{label}Success! Average: {average_error}")
        else:
            print(f"{label}Failure! Average: {average_error}")


if __name__ == "__main__":
    t = Test()
    t.testbench(0)
    t.testbench(1)
