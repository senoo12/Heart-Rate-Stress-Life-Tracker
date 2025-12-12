from django.urls import path
from .views_device import set_active_user

urlpatterns = [
    path("set-active-user/", set_active_user)
]