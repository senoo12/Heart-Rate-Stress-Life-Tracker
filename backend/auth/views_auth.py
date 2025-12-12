from rest_framework_simplejwt.views import TokenObtainPairView
from .serializers_auth import *

class LoginView(TokenObtainPairView):
    serializer_class = TokenObtainPairSerializer