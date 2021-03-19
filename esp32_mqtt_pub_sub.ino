/*
Connect to MQTT broker and publish with json
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

//You're wifi credentials goes here
#define WIFI_SSID "you're ssid"
#define WIFI_PASSWORD "you're wifi password"
//The username and password registered on the broker goes here
#define MQTT_USERNAME "username to access the broker"
#define MQTT_PASSWORD "password to access the broker"
//The server name, or domain or ip registed on the certificate
#define MQTT_SERVER "server domain/ip/name"
//Set topics for subscription and publish
#define PUBLISH_TOPIC "publish/topic"
#define SUBSCRIBE_TOPIC "subscribe/topic"

//Create an wifi secure and mqtt client object
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void wifiReconnect(){
/*
The wifiReconnect function connects the ESP32 to the especified wifi each time it's called
*/
  int counter = 0;
  Serial.print("Connecting to the network: ");
  Serial.println(WIFI_SSID);
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
}

void mqttReconnect(){
/*
The mqttReconnect function establishes the connection to the broker each time it's called
  - A client ID is generated each time a new connection is requiered, to make it's value is unique
*/
  char* clientId = "ESP32_"; //Genreate a random value to make sure it is the only id connected to the broker
  while(!mqttClient.connected()){
    Serial.print("Connecting to broker... ");
    if(mqttClient.connect(clientId, MQTT_USERNAME, MQTT_PASSWORD)){
      Serial.println("Connected");
      mqttClient.subscribe(SUBSCRIBE_TOPIC);
    }
    else{
      Serial.print("Failed to connect -> ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length){
/*
The callback function processes the messages recieved from broker
  - This is a simple example were the message is stored on a Strig and printed in the terminal
*/
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

void setup() {
  Serial.begin(115200);
  mqttClient.setServer(MQTT_SERVER, 8883);
  mqttClient.setCallback(callback);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED)
    wifiReconnect();
  mqttReconnect();
  //From now on you can publish the way you like. Don't forget to disconnect after making a publish
  mqttClient.publish(PUBLISH_TOPIC, "you're message");
  mqttClient.disconnect();
  // The loop function ensures the mqtt receives messages from the broker on the subscribed topics
  mqttClient.loop();
  delay(10000);
}
