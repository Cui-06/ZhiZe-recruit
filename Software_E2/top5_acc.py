from pathlib import Path

import torch

from cnn import CNN
from main import MLP
from utils import build_loaders, get_device, topk_accuracy


def load_model(checkpoint_path: Path):
    # Rebuild the correct model class from checkpoint metadata.
    checkpoint = torch.load(checkpoint_path, map_location="cpu")
    model_name = checkpoint["model_name"]
    config = checkpoint.get("config", {})

    if model_name == "mlp":
        model = MLP(
            hidden1=int(config.get("hidden1", 512)),
            hidden2=int(config.get("hidden2", 256)),
            dropout=float(config.get("dropout", 0.2)),
        )
        flatten = True
    elif model_name == "cnn":
        model = CNN(dropout=float(config.get("dropout", 0.25)))
        flatten = False
    else:
        raise ValueError(f"unknown model_name: {model_name}")

    model.load_state_dict(checkpoint["model_state_dict"])
    return model, flatten


def report_topk(model_path: Path) -> None:
    # Load a trained model and report Top-1 through Top-5 accuracy.
    device = get_device()
    model, flatten = load_model(model_path)
    model.to(device)
    _, test_loader = build_loaders(batch_size=256, flatten=flatten)
    scores = topk_accuracy(model, test_loader, device, max_k=5)

    print(f"\n{model_path}")
    for k, acc in scores.items():
        print(f"Top-{k} accuracy: {acc:.4f}")


def main() -> None:
    model_dir = Path("model")
    for path in [model_dir / "mlp.pth", model_dir / "cnn.pth"]:
        if path.exists():
            report_topk(path)
        else:
            print(f"skip missing checkpoint: {path}")

    print(
        "\nTop-k accuracy: the prediction is counted as correct if the true label "
        "appears in the first k classes sorted by model probability."
    )


if __name__ == "__main__":
    main()
