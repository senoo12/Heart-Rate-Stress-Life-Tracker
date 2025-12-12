from django.db import models
from django.contrib.auth.models import *

# Create your models here.
class Fisiologis(models.Model):
    rmssd = models.FloatField()
    heart_rate = models.FloatField()
    spo2 = models.FloatField()
    pnn50 = models.FloatField()
    sdrr = models.FloatField()
    predict_cluster = models.IntegerField(null=True)
    label = models.CharField(max_length=20, null=True)
    user_id = models.ForeignKey(User, on_delete=models.CASCADE, null=True)

    created_at = models.DateTimeField(auto_now_add=True)