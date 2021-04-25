/*
 * Connect to MQTT broker and publishes with json
 */

#include <Arduino.h>
#include <driver/adc.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//Used to read the adc
#define ADC_MIN 0
#define ADC_MAX 4095
#define SAMPLES 3
#define SAMPLE_DELAY 1000 * 5

//Genreate a random value to make sure it is the only id connected to the broker
#define CLIENT_ID "ESP32_asdfasbhdfasdfuashdfpipoask-q9ju"
//Set topics for subscription and publish
#define PUBLISH_TOPIC "sensor/gas"
#define SUBSCRIBE_TOPIC "sensor/gas"
//You're wifi credentials goes here
const char* WIFI_SSID = "Ribeiro_2.4G";
const char* WIFI_PASSWORD = "99955015";
//The server name, or domain or ip registed on the certificate and secure port
const char* BROKER_SERVER = "192.168.0.11";
int BROKER_PORT = 8883;
//The username and password registered on the broker goes here
const char* MQTT_USERNAME = "controller";
const char* MQTT_PASSWORD = "esp32";

//Create an wifi secure and mqtt client object
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

/*
 * The mics6814Init function initializes the adc channels where the sensor is connected
 */
bool mics6814Init(){
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
  return true;
}

/*
 * The mics6814Read function read 3 values from the adc and returns its mean
 */
bool mics6814Read(uint16_t* co, uint16_t* no2, uint16_t* nh3){
  uint16_t readValue;
  uint16_t coRead = 0;
  uint16_t no2Read = 0;
  uint16_t nh3Read = 0;
  uint8_t count = 0;

  while(count < SAMPLES){
    readValue = adc1_get_raw(ADC1_CHANNEL_4);
    if(readValue >= ADC_MIN && readValue <= ADC_MAX)
      coRead += (uint16_t)readValue;
    else
      return false;
    readValue = adc1_get_raw(ADC1_CHANNEL_6);
    if(readValue >= ADC_MIN && readValue <= ADC_MAX)
      no2Read += (uint16_t)readValue;
    else
      return false;
    readValue = adc1_get_raw(ADC1_CHANNEL_7);
    if(readValue >= ADC_MIN && readValue <= ADC_MAX)
      nh3Read += (uint16_t)readValue;
    else
      return false;
    count++;
    if(count != 2)
      delay(SAMPLE_DELAY);
  }
  *co = ADC_MAX - (coRead/SAMPLES);
  *no2 = ADC_MAX - (no2Read/SAMPLES);
  *nh3 = ADC_MAX - (nh3Read/SAMPLES);
  return true;
}


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
  uint16_t co = 0;
  uint16_t no2 = 0;
  uint16_t nh3 = 0;

  if(mics6814Read(&co, &no2, &nh3)){
    StaticJsonDocument<96> doc;
    doc["sensor"] = "gas";
    doc["co"] = co;
    doc["no2"] = no2;
    doc["nh3"] = nh3;
    char buffer[96];
    serializeJson(doc, buffer);
    MQTTReconnect();
    Serial.println("Sending message... ");
    Serial.println(buffer);
    mqttClient.publish(PUBLISH_TOPIC, buffer);
    mqttClient.disconnect();
  }
}

//Setup function
void setup() {
  Serial.begin(115200);
  if(!mics6814Init())
    Serial.println("Failed to start the sensor");
  wifiInit();
  MQTTInit();
}

//Loop function
void loop() {
  if(WiFi.status() != WL_CONNECTED) // If the wifi connection was lost, reconnects
    wifiInit();
  MQTTPublish();
  // The loop function ensures the mqtt receives messages from the broker on the subscribed topics
  mqttClient.loop();
  delay(1000 * 10); // Waits 10 seconds
}
