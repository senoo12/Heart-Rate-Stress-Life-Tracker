from django.http import JsonResponse
from device.models_device import *

class DeviceAuthMiddleware:
    def __init__(self, get_response):
        self.get_response = get_response

    def __call__(self, request):

        if request.path.startswith("/api/predict/"):
            api_key = request.headers.get("X-API-KEY")

            if not api_key: 
                return JsonResponse({"error": "Missing API key"}, status=401)

            device = Device.objects.filter(api_key=api_key, is_active=True).first()

            if not device:
                return JsonResponse({"error": "Invalid API key"}, status=403)

            # Load user aktif dari device
            active = ActiveDeviceUser.objects.filter(device_id=device).first()

            if not active:
                return JsonResponse({"error": "No active user for this device"}, status=403)

            request.device = device
            request.user_from_device = active.user_id

        return self.get_response(request)