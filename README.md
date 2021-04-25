# esp32-mqtt
This is a smart city college project.

The microcontroller used was a ESP32 with a LoRaWAN module.

The MQTT server was created using the Mosquitto broker on a Raspberry Pi.

## Create the broker and secure the connection with TLS:
### Installing the broker
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install mosquitto
sudo apt-get install mosquitto-clients
```

### Generating passwords file
```bash
sudo mosquitto_passwd -c /etc/mosquitto/conf.d/passwords user_name
```
Users can be added to the file passwords removing the -c argument:
```bash
sudo mosquitto_passwd /etc/mosquitto/conf.d/passwords new_user_name
```
The documentation can be found [here](https://mosquitto.org/man/mosquitto_passwd-1.html).

### Creatint a ACL file for topic permission control
```bash
sudo nano /etc/mosquitto/conf.d/aclfile.txt
```
Edit the file as you need, the documentation can be found [here](https://mosquitto.org/man/mosquitto-conf-5.html) in the General Options section.

### Configuring secure connection with TLS
The certificates were generated usin OpenSSL (comes pre installed with Ubuntu 18.04).

First, we need to generate the certificate authority certificate and key:
```bash
sudo openssl genrsa -des3 -out ca.key 2048
sudo openssl req -new -x509 -days 1826 -key ca.key -out ca.crt
```
Be sure to type either your MQTT server domain, the pc name or the IP when the terminal asks for the Common Name.

Then we need to generate a server key, a certificate signing request, and then sign it with our authority key:
```bash
sudo openssl genrsa -out server.key 2048
sudo openssl req -new -out server.csr -key server.key
sudo openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 360
```
Again, type either your MQTT server domain, the pc name or the IP when the terminal asks for the Common Name.

### Copying the certificates created to the Mosquitto folder
- ca.crt goes to /etc/mosquitto/ca_certificates
- server.crt and server.key goes to /etc/mosquitto/certs

### Edit the configuration file
```bash
sudo nano /etc/mosquitto/conf.d/mosquitto.conf
```
Add to the file the following lines (leave a blank line in the end):
```
listener 8883
certfile /etc/mosquitto/certs/server.crt
keyfile /etc/mosquitto/certs/server.key
cafile /etc/mosquitto/ca_certificates/ca.crt
allow_anonymous false
password_file /etc/mosquitto/conf.d/passwords
acl_file /etc/mosquitto/conf.d/aclfile.txt
```

### Restartng the broker
```
sudo systemctl restart mosquitto
```

After that, you shoud have a working secure MQTT server.

I tested the encryption using Wireshark listening on port 8883 and the data were encrypted usyng TLSv1.2.

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
