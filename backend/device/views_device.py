from rest_framework.decorators import api_view, permission_classes
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from device.models_device import *

@api_view(["POST"])
@permission_classes([IsAuthenticated])
def set_active_user(request):
    device_name = request.data.get("device_name")

    if not device_name:
        return Response({"error": "device_name required"}, status=400)

    # Ambil user dari JWT token
    user = request.user

    # Cari device berdasarkan nama
    device = Device.objects.filter(name=device_name).first()
    if not device:
        return Response({"error": "Device not found"}, status=404)

    # Update atau buat active device user
    ActiveDeviceUser.objects.update_or_create(
        device_id=device,
        defaults={"user_id": user}
    )

    return Response({
        "message": "Active user updated successfully",
        "device": device.name,
        "user": user.username
    })
