import json
from django.shortcuts import render
from .models import Temperature, Humidity, Pressure, Tvoc, Co2

def graphs(request):
    queryset_temp = Temperature.objects.all().order_by('-id')[:48][::-1]
    temp_values = [float(obj.value) for obj in queryset_temp]
    temp_times = ["{}:{}".format(obj.created_at.hour, obj.created_at.minute) for obj in queryset_temp]

    queryset_humi = Humidity.objects.all().order_by('-id')[:48][::-1]
    humi_values = [float(obj.value) for obj in queryset_humi]
    humi_times = ["{}:{}".format(obj.created_at.hour, obj.created_at.minute) for obj in queryset_humi]

    queryset_press = Pressure.objects.all().order_by('-id')[:48][::-1]
    press_values = [float(obj.value) for obj in queryset_press]
    press_times = ["{}:{}".format(obj.created_at.hour, obj.created_at.minute) for obj in queryset_press]

    queryset_co2 = Co2.objects.all().order_by('-id')[:48][::-1]
    co2_values = [int(obj.value) for obj in queryset_co2]
    co2_times = ["{}:{}".format(obj.created_at.hour, obj.created_at.minute) for obj in queryset_co2]
    
    queryset_tvoc = Tvoc.objects.all().order_by('-id')[:48][::-1]
    tvoc_values = [int(obj.value) for obj in queryset_tvoc]
    tvoc_times = ["{}:{}".format(obj.created_at.hour, obj.created_at.minute) for obj in queryset_tvoc]

    context = {
        'temp_values': json.dumps(temp_values),
        'temp_times': json.dumps(temp_times),
        'humi_values': json.dumps(humi_values),
        'humi_times': json.dumps(humi_times),
        'press_values': json.dumps(press_values),
        'press_times': json.dumps(press_times),
        'co2_values': json.dumps(co2_values),
        'co2_times': json.dumps(co2_times),
        'tvoc_values': json.dumps(tvoc_values),
        'tvoc_times': json.dumps(tvoc_times),
    }
    return render(request, 'web/graficos.html', context)