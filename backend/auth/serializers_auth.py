from rest_framework_simplejwt.serializers import TokenObtainPairSerializer
from device.models_device import Device, ActiveDeviceUser

class TokenObtainPairSerializer(TokenObtainPairSerializer):

    @classmethod
    def get_token(cls, user):
        token = super().get_token(user)
        token['username'] = user.username
        return token

    def validate(self, attrs):
        # Hasil validasi login standar JWT
        data = super().validate(attrs)

        # User yang berhasil login
        user = self.user
        
        # Ambil device_name dari request body saat login
        device_name = self.context["request"].data.get("device_name")

        if device_name:
            # Cari device
            device = Device.objects.filter(name=device_name).first()

            if device:
                # Update active device user
                ActiveDeviceUser.objects.update_or_create(
                    device_id=device,
                    defaults={"user_id": user}
                )
                data["active_device"] = device_name
            else:
                data["active_device"] = None
                data["warning"] = "Device not found in database"
        else:
            data["active_device"] = None
            data["warning"] = "device_name is required to update active user"

        # Tambahkan username ke response login
        data['username'] = user.username

        return data
