from django.contrib import admin
from django.urls import path
from web import views

from web import mqtt
mqtt.run_client()

urlpatterns = [
    path('admin/', admin.site.urls),
    path('temperature/', views.temperature, name='temperature'),
]
