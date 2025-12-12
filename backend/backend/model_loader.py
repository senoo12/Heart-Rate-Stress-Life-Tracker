import os
import joblib

def load_model():
    # Ambil direktori root project (folder yang berisi manage.py)
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    # Path ke folder models
    model_path = os.path.join(base_dir, "models", "heart_rate_classification_model.pkl")

    if not os.path.exists(model_path):
        raise FileNotFoundError(f"Model not found at: {model_path}")

    return joblib.load(model_path)
