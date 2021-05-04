# esp32-mqtt
This is a smart city college project.

The microcontroller used was a ESP32 with a LoRaWAN module.

The MQTT server was created using the Mosquitto broker on a Raspberry Pi.

## TODO
- [ ] Fix no2 and nh3 readings
- [ ] Improve gas readings
- [ ] Add Arduino IDE documentatation
- [ ] Add ACL documentation

## Configuring the Arduino IDE to work with ESP32
### Edit Arduino preferences
Go to File > Preferences;

Add https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json to the “Additional Board Manager URLs” and save.

### Download the library
Go to Tools > Board > Boards Manager…;

Search for ESP32 and install “ESP32 by Espressif Systems“;

I had troubles connecting to the broker working with the latest version (1.0.6). 1.0.4 is working fine in my case. Still needs to discover why I can't connect with 1.0.6.

### Select your board
Go to Tools > Board > ESP32 Arduino;

Select your board;

In my case: Heltec WiFi LoRa 32.

### Select the right COM port
Go to Tools > Port;

Select your corresponding COM Port:

You can check on Windows using Device Manager;

You can check on Ubuntu running;
```
ls /dev/tty*
```
Without the board plugged and later on with the board plugged. The port which appears the second time is the one you need;

If you try to run the project and it denies acces to the port (Ubuntu), run this code:
```
sudo chmod a+rw /dev/[your COM port]
```

## Libraries used
You can download them on Tools > Manage Libraries...

### [PubSubClient by Nick O' Leary](https://github.com/knolleary/pubsubclient)
I'm using the latest version avaliable (2.8.0).

### [ArduinoJson by Benoit Blanchon](https://github.com/bblanchon/ArduinoJson)
I'm using the latest version avaliable (6.17.3).
