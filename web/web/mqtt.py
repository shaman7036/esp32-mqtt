import json
import paho.mqtt.client as mqtt

TEMP = "school-monitor/sensor/bme280-temperature/state"
HUMIDITY = "school-monitor/sensor/bme280-humidity/state"
PRESSURE = "school-monitor/sensor/bme280-pressure/state"
CO2 = "school-monitor/sensor/ccs811-co2/state"
TVOC = "school-monitor/sensor/ccs811-tvoc/state"

MQTT_TOPICS = [ (TEMP, 0), (HUMIDITY, 0),
              (PRESSURE, 0), (CO2, 0),
              (TVOC, 0) ]

def on_connect(client, userdata, flags, rc):
    print("CONNACK received with code %d." % (rc))
    client.subscribe(MQTT_TOPICS)

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_message(client, userdata, msg):
    from .models import Temperature, Humidity, Pressure, Tvoc, Co2

    print(msg.payload)

    message_dict = json.loads(msg.payload)

    if msg.topic == TEMP:
        Temperature(value=message_dict).save()
        print("temperature saved")
    elif msg.topic == HUMIDITY:
        Humidity(value=message_dict).save()
        print("humidity saved")
    elif msg.topic == PRESSURE:
        Pressure(value=message_dict).save()
        print("pressure saved")
    elif msg.topic == CO2:
        Co2(value=message_dict).save()
        print("co2 saved")
    elif msg.topic == TVOC:
        Tvoc(value=message_dict).save()
        print("tvoc saved")

def run_client():
    client = mqtt.Client("django_paho_mqtt_subscriber", clean_session=True, userdata=None)
    client.on_connect = on_connect
    client.on_subscribe = on_subscribe
    client.on_message = on_message

    client.username_pw_set(username="user1",password="gabriel")
    client.connect_async(host="iot.coenc.ap.utfpr.edu.br", port=1883, keepalive=60, bind_address="")
    client.loop_start()
