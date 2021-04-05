/*
Connect to MQTT broker and publish with json
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//Set topics for subscription and publish
#define PUBLISH_TOPIC "iot/sensor"
#define SUBSCRIBE_TOPIC "iot/sensor"

//Genreate a random value to make sure it is the only id connected to the broker
#define CLIENT_ID "ESP32_123"

//You're wifi credentials goes here
const char* WIFI_SSID = "Ribeiro";
const char* WIFI_PASSWORD = "ribeiro457";
//The server name, or domain or ip registed on the certificate and secure port
const char* BROKER_SERVER = "192.168.1.105";
int BROKER_PORT = 8883;
//The username and password registered on the broker goes here
const char* MQTT_USERNAME = "controlador";
const char* MQTT_PASSWORD = "esp32";

//Create an wifi secure and mqtt client object
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

/*
 * The initWifi function initializes and connects to wifi
 */
void wifiInit(){
  delay(10);
  Serial.print("Connecting to the network: ");
  Serial.println(WIFI_SSID);
  wifiReconnect();
}

/*
 * The initMQTT sets the broker parameters (address, port and callback)
 */
void MQTTInit(){
  mqttClient.setServer(BROKER_SERVER, BROKER_PORT);
  mqttClient.setCallback(callback);
}

/*
 * The callback function processes the messages recieved from broker
 * - This is a simple example were the message is stored on a Strig and printed in the terminal
 */
void callback(char* topic, byte* payload, unsigned int length){
  char c;
  String message;
  int i;
  for(i = 0; i < length; i++){
    c = (char)payload[i];
    message += c;
  }
  Serial.print("Message recieved from broker: ");
  Serial.print(message);
}

/*
 * The mqttReconnect function establishes the connection to the broker each time it's called
 */
void MQTTReconnect(){
  while(!mqttClient.connected()){
    Serial.print("Connecting to broker... ");
    if(mqttClient.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)){
      mqttClient.subscribe(SUBSCRIBE_TOPIC);
      Serial.println("Connected");
    }
    else{
      Serial.print("Failed to connect -> ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

/*
 * The wifiReconnect function connects the ESP32 to the especified wifi each time it's called
 */
void wifiReconnect(){
  int counter = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.print(".");
    counter++;
    if(counter > 50){
       Serial.println(" Can't connect, restarting ESP32...");
       delay(3000);
       ESP.restart();
    }
  }
  Serial.println(" Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

/*
 * The MQTTPublish gets the values from the sensors, creates a Json formatted output and publishes it
 * In this case, the sensor values were randomic
 */
void MQTTPublish(){
  DynamicJsonDocument json(1024);
  json["sensor"] = "temp";
  json["time"] = "2021-03-31T11:26Z";
  json["data"] = 24;
  char buffer[256];
  serializeJson(json, buffer);
  MQTTReconnect();
  Serial.println("Sending message... ");
  Serial.println(buffer);
  mqttClient.publish(PUBLISH_TOPIC, buffer);
  mqttClient.disconnect();
}

//Setup function
void setup() {
  Serial.begin(115200);
  wifiInit();
  MQTTInit();
}

//Loop function
void loop() {
  if(WiFi.status() != WL_CONNECTED) //if the wifi connection was lost, reconnects
    wifiInit();
  MQTTPublish();
  // The loop function ensures the mqtt receives messages from the broker on the subscribed topics
  mqttClient.loop();
  delay(10000);
}
