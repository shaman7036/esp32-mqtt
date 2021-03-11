# esp32-mqtt
This is a smart city college project.

The microcontroller used was a ESP32 with a LoRaWAN module.

The MQTT server created using the broker Mosquitto on Ubuntu 18.04LTS.

## Steps to create the broker and secure the connection with TLS:
### Install the broker
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install mosquitto
sudo apt-get install mosquitto-clients
```
### Generate passwords file
```bash
sudo mosquitto_passwd -c /etc/mosquitto/conf.d/passwords user_name
```
Users can be added to the file passwords removing the -c argument:
```bash
sudo mosquitto_passwd /etc/mosquitto/conf.d/passwords new_user_name
```
The documentation can be found [here](https://mosquitto.org/man/mosquitto_passwd-1.html).
### Create ACL file for topic permission control
```bash
sudo gedit /etc/mosquitto/conf.d/aclfile.acl
```
Edit the file as you need, the documentation can be found [here](https://mosquitto.org/man/mosquitto-conf-5.html) in the General Options section.
### Configure secure connection with TLS
The certificates were generated usin OpenSSL (comes pre installed with Ubuntu 18.04).

First, we need to generate the certificate authority certificate and key:
```bash
sudo openssl genrsa -des3 -out ca.key 2048
sudo openssl req -new -x509 -days 1826 -key ca.key -out ca.crt
```
Be sure to type either your MQTT server domain or the name of your pc when the terminal asks for the Common Name.

Then we need to generate a server key, a certificate signing request, and then sign it with our authority key:
```bash
sudo openssl genrsa -out server.key 2048
sudo openssl req -new -out server.csr -key server.key
sudo openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 360
```
Again, type either your MQTT server domain or the name of your pc when the terminal asks for the Common Name.

Finally we generate the fingerprint of the server:
```bash
sudo openssl x509 -in server.crt -sha1 -noout -fingerprint
```
### Copy the certificates created to the Mosquitto folder
ca.crt goes to /etc/mosquitto/ca_certificates

server.crt and server.key goes to /etc/mosquitto/certs

### Edit the configuration file
```bash
sudo gedit /etc/mosquitto/mosquitto.conf
```
Add to the file the following lines (leave a blank line in the end):
```
port 8883
allow_anonymous false
password_file /etc/mosquitto/conf.d/passwords
acl_file /etc/mosquitto/conf.d/aclfile.txt
cafile /etc/mosquitto/ca_certificates/ca.crt
certfile /etc/mosquitto/certs/server.crt
keyfile /etc/mosquitto/certs/server.key
tls_version tlsv1

```
After that, you shoud have a working secure MQTT server.

To test it, you can open two terminal tabs and make a subscribe in one tab and a publish in the other tab, to a test topic:
```
mosquitto_sub -h localhost -p 8883 -u "username" -P "password" -t test --cafile /etc/mosquitto/ca_certificates/ca.crt
mosquitto_pub -h localhost -p 8883 -u "username" -P "password" -t test -m "message" --cafile /etc/mosquitto/ca_certificates/ca.crt
```
