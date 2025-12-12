from rest_framework import serializers

class StressInputSerializer(serializers.Serializer):
    rmssd = serializers.FloatField()
    heart_rate = serializers.FloatField()
    spo2 = serializers.FloatField()
    pnn50 = serializers.FloatField()
    sdrr = serializers.FloatField()
