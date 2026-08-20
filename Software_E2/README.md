# Software_E2 手写数字识别

本目录完成 MNIST 手写数字识别任务，包含 MLP、CNN、Top-k 准确率评估代码和实验报告。

数据来源：MNIST handwritten digit database，原始数据由 Yann LeCun、Corinna Cortes 和 Christopher J.C. Burges 整理发布；本题使用配套提供的 `mnist_x.txt` 和 `mnist_y.txt`。

## 环境

- Python 3.11
- PyTorch
- NumPy

本机可使用 `D:\Anaconda\envs\py311_env\python.exe` 运行。

## 文件结构

```text
Software_E2/
├── mnist_x.txt
├── mnist_y.txt
├── utils.py
├── main.py
├── cnn.py
├── top5_acc.py
├── model/
│   ├── mlp.pth
│   └── cnn.pth
 └── report.md
└── 学习文档.md
```

## 运行方式

建议使用 `D:\Anaconda\envs\py311_env\python.exe`。脚本也会在默认解释器下自动切换到该环境。

训练 MLP：

```bash
python main.py
```

训练 CNN：

```bash
python cnn.py
```

输出 Top-1 到 Top-5 准确率：

```bash
python top5_acc.py
```

## 说明

数据集按 60000/10000 划分训练集和测试集。所有脚本默认从当前目录读取 `mnist_x.txt` 和 `mnist_y.txt`，模型权重保存到 `model/`。
