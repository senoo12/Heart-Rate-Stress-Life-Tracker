from django.contrib import admin
from django.urls import path, include
from django.conf import settings
from django.conf.urls.static import static

urlpatterns = [
    path('admin/', admin.site.urls),
    path('api/', include("api.urls_api")),
    path('auth/', include("auth.urls_auth")),
    path('device/', include("device.urls_device")),
] + static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
