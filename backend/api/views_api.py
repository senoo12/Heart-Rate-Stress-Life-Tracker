import numpy as np
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status
from backend.model_loader import load_model
from api.models_api import *

model = load_model()

label_map = {
    0: "medium",
    1: "low",
    2: "high"
}

@api_view(["POST"])
def predict_cluster(request):
    try:
        user = request.user_from_device
        data = request.data

        rmssd = float(data.get("rmssd"))
        sdrr = float(data.get("sdrr"))
        pnn50 = float(data.get("pnn50"))
        heart_rate = float(data.get("heart_rate"))
        spo2 = float(data.get("spo2"))

        X = [[
            np.log(rmssd),
            np.log(heart_rate),
            np.log(spo2),
            np.log(pnn50),
            np.log(sdrr)
        ]]

        prediction = model.predict(X)[0]
        mapped_label = label_map[int(prediction)]

        # Simpan ke database
        Fisiologis.objects.create(
            rmssd=rmssd,
            sdrr=sdrr,
            pnn50=pnn50,
            heart_rate=heart_rate,
            spo2=spo2,
            predict_cluster=int(prediction),
            label=mapped_label,
            user_id=user,
        )

        return Response({
            "predicted_cluster": int(prediction),
            "label": mapped_label
        })

    except Exception as e:
        return Response(
            {"error": str(e)},
            status=status.HTTP_400_BAD_REQUEST
        )
