from django.contrib import admin
from .models_api import Fisiologis

class FisiologisAdmin(admin.ModelAdmin):
    list_display = ('rmssd', 'heart_rate', 'spo2', 'pnn50', 'sdrr', 'predict_cluster', 'label', 'created_at')
    readonly_fields = ('created_at',)

    # User hanya dapat melihat datanya sendiri
    def get_queryset(self, request):
        qs = super().get_queryset(request)

        # Superuser bisa lihat semuanya
        if request.user.is_superuser:
            return qs
        
        # User biasa hanya lihat datanya sendiri
        return qs.filter(user_id=request.user)

    # User tidak bisa melihat ataupun mengedit data user lain via URL
    def get_object(self, request, object_id, from_field=None):
        obj = super().get_object(request, object_id)

        # Superuser bebas
        if request.user.is_superuser:
            return obj

        # Kalau bukan pemilik → anggap data tidak ada
        if obj and obj.user_id != request.user:
            return None

        return obj

    # Owner otomatis → user yang login
    def save_model(self, request, obj, form, change):
        if not change:  
            obj.user_id = request.user
        obj.save()

    # Hide field user_id di admin untuk user biasa
    def get_fields(self, request, obj=None):
        fields = super().get_fields(request, obj)
        if not request.user.is_superuser:
            if 'user_id' in fields:
                fields.remove('user_id')
        return fields

    # Batasi permission supaya user tidak bisa hapus atau ubah data orang lain
    def has_change_permission(self, request, obj=None):
        if obj is None:
            return True
        
        if request.user.is_superuser:
            return True
        
        return obj.user_id == request.user

    def has_delete_permission(self, request, obj=None):
        if obj is None:
            return True
        
        if request.user.is_superuser:
            return True

        return obj.user_id == request.user


admin.site.register(Fisiologis, FisiologisAdmin)
