from django.urls import path
from .views_api import predict_cluster

urlpatterns = [
    path("predict/", predict_cluster),
]
