from django.db import models
from django.contrib.auth.models import *

# Create your models here.
class Device(models.Model):
    name = models.CharField(max_length=180, unique=True, default="ESP32 Device")
    api_key = models.CharField(max_length=40, unique=True)
    is_active = models.BooleanField(default=True)
    crated_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.name

class ActiveDeviceUser(models.Model):
    device_id = models.OneToOneField(Device, on_delete=models.CASCADE)
    user_id = models.ForeignKey(User, on_delete=models.CASCADE)
    updated_at = models.DateTimeField(auto_now=True)
    
    def __str__(self):
        return f"{self.device_id.name} → {self.user_id.username}"