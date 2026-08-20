import random
import time
from pathlib import Path
from typing import Dict, Tuple

import torch
from torch import nn
from torch.utils.data import DataLoader, TensorDataset, random_split


# Project root and local MNIST text files.
ROOT = Path(__file__).resolve().parent
DATA_X = ROOT / "mnist_x.txt"
DATA_Y = ROOT / "mnist_y.txt"


def set_seed(seed: int = 42) -> None:
    # Keep training results reproducible as much as possible.
    random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.benchmark = False
    torch.backends.cudnn.deterministic = True


def get_device() -> torch.device:
    # Use GPU when the selected Python environment supports it; otherwise use CPU.
    return torch.device("cuda" if torch.cuda.is_available() else "cpu")


def load_mnist_tensors(flatten: bool = True) -> Tuple[torch.Tensor, torch.Tensor]:
    # Read text-format MNIST and normalize pixel values from 0-255 to 0-1.
    with DATA_X.open("r", encoding="utf-8") as fx:
        x_rows = [[int(v) / 255.0 for v in line.split()] for line in fx if line.strip()]
    with DATA_Y.open("r", encoding="utf-8") as fy:
        y_rows = [int(line.strip()) for line in fy if line.strip()]

    # MLP uses flat 784-dim vectors; CNN uses 1 x 28 x 28 image tensors.
    if flatten:
        features = torch.tensor(x_rows, dtype=torch.float32)
    else:
        features = torch.tensor(x_rows, dtype=torch.float32).view(-1, 1, 28, 28)
    labels = torch.tensor(y_rows, dtype=torch.long)
    return features, labels


def build_loaders(
    batch_size: int = 128,
    flatten: bool = True,
    train_size: int = 60000,
    seed: int = 42,
) -> Tuple[DataLoader, DataLoader]:
    # Split data into 60000 training samples and 10000 test samples.
    features, labels = load_mnist_tensors(flatten=flatten)
    dataset = TensorDataset(features, labels)
    test_size = len(dataset) - train_size
    generator = torch.Generator().manual_seed(seed)
    train_set, test_set = random_split(dataset, [train_size, test_size], generator=generator)

    train_loader = DataLoader(train_set, batch_size=batch_size, shuffle=True)
    test_loader = DataLoader(test_set, batch_size=batch_size, shuffle=False)
    return train_loader, test_loader


def accuracy_from_logits(logits: torch.Tensor, labels: torch.Tensor) -> float:
    # Standard Top-1 accuracy.
    preds = logits.argmax(dim=1)
    return (preds == labels).float().mean().item()


@torch.no_grad()
def evaluate(model: nn.Module, loader: DataLoader, device: torch.device) -> Tuple[float, float]:
    # Evaluate loss and accuracy without gradient updates.
    model.eval()
    criterion = nn.CrossEntropyLoss()
    total_loss = 0.0
    correct = 0
    total = 0

    for features, labels in loader:
        features = features.to(device)
        labels = labels.to(device)
        logits = model(features)
        loss = criterion(logits, labels)
        total_loss += loss.item() * labels.size(0)
        correct += (logits.argmax(dim=1) == labels).sum().item()
        total += labels.size(0)

    return total_loss / total, correct / total


@torch.no_grad()
def topk_accuracy(
    model: nn.Module,
    loader: DataLoader,
    device: torch.device,
    max_k: int = 5,
) -> Dict[int, float]:
    # Top-k is correct when the true label appears in the first k predictions.
    model.eval()
    correct = {k: 0 for k in range(1, max_k + 1)}
    total = 0

    for features, labels in loader:
        features = features.to(device)
        labels = labels.to(device)
        logits = model(features)
        _, pred = logits.topk(max_k, dim=1)
        matches = pred.eq(labels.view(-1, 1))
        for k in correct:
            correct[k] += matches[:, :k].any(dim=1).sum().item()
        total += labels.size(0)

    return {k: correct[k] / total for k in correct}


def train_model(
    model: nn.Module,
    train_loader: DataLoader,
    test_loader: DataLoader,
    device: torch.device,
    epochs: int,
    lr: float,
    weight_decay: float = 0.0,
) -> Dict[str, float]:
    # Shared training loop for both MLP and CNN.
    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=lr, weight_decay=weight_decay)
    model.to(device)

    started = time.perf_counter()
    final_train_loss = 0.0
    final_train_acc = 0.0
    final_test_loss = 0.0
    final_test_acc = 0.0

    for epoch in range(1, epochs + 1):
        model.train()
        running_loss = 0.0
        running_correct = 0
        total = 0

        for features, labels in train_loader:
            features = features.to(device)
            labels = labels.to(device)

            optimizer.zero_grad()
            logits = model(features)
            loss = criterion(logits, labels)
            loss.backward()
            optimizer.step()

            running_loss += loss.item() * labels.size(0)
            running_correct += (logits.argmax(dim=1) == labels).sum().item()
            total += labels.size(0)

        final_train_loss = running_loss / total
        final_train_acc = running_correct / total
        test_loss, test_acc = evaluate(model, test_loader, device)
        final_test_loss = test_loss
        final_test_acc = test_acc
        print(
            f"epoch {epoch:02d}/{epochs} "
            f"train_loss={final_train_loss:.4f} train_acc={final_train_acc:.4f} "
            f"test_loss={test_loss:.4f} test_acc={test_acc:.4f}"
        )

    return {
        "train_loss": final_train_loss,
        "train_acc": final_train_acc,
        "test_loss": final_test_loss,
        "test_acc": final_test_acc,
        "train_seconds": time.perf_counter() - started,
    }


def save_checkpoint(
    path: Path,
    model: nn.Module,
    model_name: str,
    config: Dict[str, float],
    metrics: Dict[str, float],
) -> None:
    # Save weights, model config, and metrics for later top-k evaluation.
    path.parent.mkdir(parents=True, exist_ok=True)
    torch.save(
        {
            "model_name": model_name,
            "model_state_dict": model.state_dict(),
            "config": config,
            "metrics": metrics,
        },
        path,
    )


def print_metrics(title: str, metrics: Dict[str, float]) -> None:
    # Print final metrics in a consistent format.
    print(f"\n{title}")
    for key, value in metrics.items():
        if key.endswith("seconds"):
            print(f"{key}: {value:.2f}s")
        else:
            print(f"{key}: {value:.4f}")
