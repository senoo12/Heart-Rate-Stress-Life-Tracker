import os
import joblib

def load_model():
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    model_path = os.path.join(
        base_dir,
        "models",
        "heart_rate_classification_model.pkl"  
    )

    if not os.path.exists(model_path):
        raise FileNotFoundError(f"Model not found at: {model_path}")

    bundle = joblib.load(model_path)

    pipeline = bundle["pipeline"]
    label_map = bundle["label_map"]

    return pipeline, label_map