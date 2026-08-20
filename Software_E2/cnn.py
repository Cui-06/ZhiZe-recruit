from pathlib import Path

import torch
from torch import nn

from utils import build_loaders, get_device, print_metrics, save_checkpoint, set_seed, train_model


class CNN(nn.Module):
    def __init__(self, dropout: float = 0.25) -> None:
        super().__init__()
        # Convolution blocks extract local stroke features from the image.
        self.features = nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=3, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.Conv2d(32, 32, kernel_size=3, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.MaxPool2d(2),
            nn.Dropout(dropout),
            nn.Conv2d(32, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(),
            nn.Conv2d(64, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(),
            nn.MaxPool2d(2),
            nn.Dropout(dropout),
        )
        # Two pooling layers reduce 28 x 28 images to 7 x 7 feature maps.
        self.classifier = nn.Sequential(
            nn.Flatten(),
            nn.Linear(64 * 7 * 7, 256),
            nn.ReLU(),
            nn.Dropout(0.5),
            nn.Linear(256, 10),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        # x shape: batch_size x 1 x 28 x 28.
        x = self.features(x)
        return self.classifier(x)


def main() -> None:
    set_seed(42)
    device = get_device()
    print(f"device: {device}")

    # Tuning notes:
    # batch_size: 128 lr: 0.0008 epochs: 4 dropout: 0.25 acc: 0.9891
    # batch_size: 128 lr: 0.0008 epochs: 8 dropout: 0.25 weight_decay: 0.0001 acc: 0.9896
    # The submitted config uses the more stable final setting.
    config = {
        "batch_size": 128,
        "lr": 0.0008,
        "epochs": 8,
        "dropout": 0.25,
        "weight_decay": 1e-4,
    }

    # CNN keeps the 2D image structure.
    train_loader, test_loader = build_loaders(batch_size=config["batch_size"], flatten=False)
    model = CNN(dropout=config["dropout"])
    metrics = train_model(
        model,
        train_loader,
        test_loader,
        device,
        epochs=config["epochs"],
        lr=config["lr"],
        weight_decay=config["weight_decay"],
    )

    save_checkpoint(Path("model") / "cnn.pth", model, "cnn", config, metrics)
    print_metrics("CNN result", metrics)
    print("saved: model/cnn.pth")


if __name__ == "__main__":
    main()
