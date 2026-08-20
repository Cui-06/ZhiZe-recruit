from pathlib import Path

import torch
from torch import nn

from utils import build_loaders, get_device, print_metrics, save_checkpoint, set_seed, train_model


class MLP(nn.Module):
    def __init__(self, hidden1: int = 512, hidden2: int = 256, dropout: float = 0.2) -> None:
        super().__init__()
        # Two hidden fully connected layers are used as the MLP baseline.
        self.net = nn.Sequential(
            nn.Linear(28 * 28, hidden1),
            nn.ReLU(),
            nn.Dropout(dropout),
            nn.Linear(hidden1, hidden2),
            nn.ReLU(),
            nn.Dropout(dropout),
            nn.Linear(hidden2, 10),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # x shape: batch_size x 784.
        return self.net(x)


def main() -> None:
    set_seed(42)
    device = get_device()
    print(f"device: {device}")

    # Tuning notes:
    # batch_size: 64 lr: 0.001 epochs: 8 hidden: 256/128 dropout: 0.0 acc: about 0.976
    # batch_size: 128 lr: 0.001 epochs: 10 hidden: 512/256 dropout: 0.2 acc: about 0.981
    # batch_size: 128 lr: 0.0005 epochs: 12 hidden: 512/256 dropout: 0.2 acc: about 0.982
    # The submitted config uses the more stable setting.
    config = {
        "batch_size": 128,
        "lr": 0.001,
        "epochs": 10,
        "hidden1": 512,
        "hidden2": 256,
        "dropout": 0.2,
        "weight_decay": 1e-5,
    }

    # MLP uses flattened 784-dim inputs.
    train_loader, test_loader = build_loaders(batch_size=config["batch_size"], flatten=True)
    model = MLP(
        hidden1=config["hidden1"],
        hidden2=config["hidden2"],
        dropout=config["dropout"],
    )
    metrics = train_model(
        model,
        train_loader,
        test_loader,
        device,
        epochs=config["epochs"],
        lr=config["lr"],
        weight_decay=config["weight_decay"],
    )

    save_checkpoint(Path("model") / "mlp.pth", model, "mlp", config, metrics)
    print_metrics("MLP result", metrics)
    print("saved: model/mlp.pth")


if __name__ == "__main__":
    main()
