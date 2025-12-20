import numpy as np
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status
from backend.model_loader import load_model
from api.models_api import *

pipeline, label_map = load_model()

@api_view(["POST"])
def predict_cluster(request):
    try:
        user = request.user_from_device
        data = request.data

        rmssd = float(data["rmssd"])
        sdrr = float(data["sdrr"])
        pnn50 = float(data["pnn50"])
        heart_rate = float(data["heart_rate"])
        spo2 = float(data["spo2"])

        X = [[
            np.log(rmssd),
            np.log(heart_rate),
            np.log(spo2),
            np.log(pnn50),
            np.log(sdrr)
        ]]

        # PREDIKSI LEWAT PIPELINE (SCALER + GMM)
        cluster = pipeline.predict(X)[0]
        label = label_map[int(cluster)]

        Fisiologis.objects.create(
            rmssd=rmssd,
            sdrr=sdrr,
            pnn50=pnn50,
            heart_rate=heart_rate,
            spo2=spo2,
            predict_cluster=int(cluster),
            label=label,
            user_id=user,
        )

        return Response({
            "predicted_cluster": int(cluster),
            "label": label
        })

    except Exception as e:
        return Response(
            {"error": str(e)},
            status=status.HTTP_400_BAD_REQUEST
        )
