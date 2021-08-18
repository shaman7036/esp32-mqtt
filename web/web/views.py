import json
from django.shortcuts import render
from .models import Temperature, Humidity, Pressure, Tvoc, Co2

def temperature(request):
    queryset = Temperature.objects.all()
    values = [int(obj.value) for obj in queryset]

    context = {
        'values': json.dumps(values),
    }
    return render(request, 'web/temperature.html', context)

#def humidity(request):
#    queryset = Humidity.objects.all()
#    values = [int(obj.value) for obj in queryset]

#    context = {
#        'values': json.dumps(values),
#    }
#    return render(request, 'web/humidity.html', context)