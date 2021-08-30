import json
from django.shortcuts import render
from .models import Temperature, Humidity, Pressure, Tvoc, Co2

def graphs(request):
    queryset_temp = Temperature.objects.all()
    temp_values = [int(obj.value) for obj in queryset_temp]

    queryset_humi = Humidity.objects.all()
    humi_values = [int(obj.value) for obj in queryset_humi]

    queryset_press = Pressure.objects.all()
    press_values = [int(obj.value) for obj in queryset_press]

    queryset_co2 = Co2.objects.all()
    press_co2 = [int(obj.value) for obj in queryset_co2]
    
    queryset_tvoc = Tvoc.objects.all()
    press_tvoc = [int(obj.value) for obj in queryset_tvoc]

    context = {
        'temp_values': json.dumps(temp_values),
        'humi_values': json.dumps(humi_values),
        'press_values': json.dumps(press_values),
        'co2_values': json.dumps(press_co2),
        'tvoc_values': json.dumps(press_tvoc),
    }
    return render(request, 'web/graficos.html', context)