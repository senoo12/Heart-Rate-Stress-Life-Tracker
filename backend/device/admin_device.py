from django.contrib import admin
from .models_device import *

# Register your models here.
admin.site.register(Device)
admin.site.register(ActiveDeviceUser)