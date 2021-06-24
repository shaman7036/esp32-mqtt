/*
 * Gas quality with mqtt integration and ota updates
 */

#include <Arduino.h>
#include <HTTPUpdate.h>
#include <driver/adc.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Sketch version
#define FIRMWARE_VERSION 1

// ADC constants
#define ADC_MIN 0
#define ADC_MAX 4095
#define SAMPLES 3
#define SAMPLE_DELAY 1000 * 5

// WiFi credentials
#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."
// Server name, domain or ip registed on the certificate and secure port
#define BROKER_ADDRESS "test.mosquitto.org"
#define BROKER_PORT 8883
// Broker credentials
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
// Topics for subscription and publishing
#define PUBLISH_TOPIC "sensor/gas"
#define SUBSCRIBE_TOPIC "ota/esp32"

const char* client_id = "d45e956a-bd6b-4d94-ae01-026e1568e1a2";

// Creates a secure wifi and mqtt client object
WiFiClientSecure wifi_client;
PubSubClient mqtt_client(wifi_client);


/*
 * Initializes the adc channels where the sensor is connected
 */
bool mics6814_init(){
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
  return true;
}

/*
 * Read the adc. Computes the mean of three readings (to improove accuracy)
 */
bool mics6814_read(uint16_t* no2, uint16_t* nh3, uint16_t* co){
  uint16_t read_value;
  uint8_t count = 0;

  while(count < SAMPLES){
    read_value = adc1_get_raw(ADC1_CHANNEL_0);
    if(read_value >= ADC_MIN && read_value <= ADC_MAX)
      *no2 += (uint16_t)read_value;
    else
      return false;
    read_value = adc1_get_raw(ADC1_CHANNEL_1);
    if(read_value >= ADC_MIN && read_value <= ADC_MAX)
      *nh3 += (uint16_t)read_value;
    else
      return false;
    read_value = adc1_get_raw(ADC1_CHANNEL_2);
    if(read_value >= ADC_MIN && read_value <= ADC_MAX)
      *co += (uint16_t)read_value;
    else
      return false;
    count++;
    if(count != 2)
      delay(SAMPLE_DELAY);
  }
  
  *no2 = ADC_MAX - (*no2/SAMPLES);
  *nh3 = ADC_MAX - (*nh3/SAMPLES);
  *co = ADC_MAX - (*co/SAMPLES);
  return true;
}


/*
 * Initializes and connects to wifi
 */
void wifi_init(){
  Serial.print("Connecting to the network: ");
  Serial.println(WIFI_SSID);
  wifi_reconnect();
}

/*
 * Connects the ESP32 to the especified wifi network
 */
void wifi_reconnect(){
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
 * Processes the messages recieved from the broker
 * - This is a simple example were the message is stored on a Strig and printed in the terminal
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length){
  String message;
  for(int i = 0; i < length; i++){
    message += (char)payload[i];
  }
  Serial.print("Message recieved from broker: ");
  Serial.println(message);

  if(message.equals("update"))
    ota_update();
}

/*
 * Sets the broker parameters (address, port and callback)
 */
void mqtt_init(){
  mqtt_client.setServer(BROKER_ADDRESS, BROKER_PORT);
  mqtt_client.setCallback(mqtt_callback);
}

/*
 * Establishes the connection to the broker
 */
void mqtt_reconnect(){
  int counter = 0;
  while(!mqtt_client.connected()){
    Serial.print("Connecting to broker... ");
    if(mqtt_client.connect(client_id, MQTT_USERNAME, MQTT_PASSWORD)){
      Serial.println("Connected");
      mqtt_client.subscribe(SUBSCRIBE_TOPIC);
    }
    else{
      Serial.print("Failed to connect -> ");
      Serial.println(mqtt_client.state());
      mqtt_client.disconnect();
      counter++;
      if(counter >= 5){
        ESP.restart();
      }
      delay(2000);
    }
  }
}

/*
 * Gets the values from the sensors, creates a Json formatted output and publishes it
 */
void mqtt_publish(){
  uint16_t no2 = 0;
  uint16_t nh3 = 0;
  uint16_t co = 0;

  if(mics6814_read(&no2, &nh3, &co)){
    StaticJsonDocument<96> json_doc;
    json_doc["sensor"] = "gas";
    json_doc["no2"] = no2;
    json_doc["nh3"] = nh3;
    json_doc["co"] = co;
    char buffer[96];
    serializeJson(json_doc, buffer);
    Serial.print("Sending message: ");
    Serial.println(buffer);
    mqtt_client.publish(PUBLISH_TOPIC, buffer);
  }
}


/*
 * Update the firmware
 */
void ota_update(){
  Serial.println("Checking for firmware updates...");

  mqtt_client.disconnect();

  HTTPClient http_client;
  http_client.begin(wifi_client, "https://www.dl.dropboxusercontent.com/s/lkt66i94xsvlab2/version_control_file.txt");
  if (http_client.GET() != HTTP_CODE_OK) {
    Serial.println("Failed to check for updates");
    return;
  }
  String version_control_file = http_client.getString();
  http_client.end();
  version_control_file.trim();

  StaticJsonDocument<200> version_control_doc;

  if (deserializeJson(version_control_doc, version_control_file)){
    Serial.println("Invalid version control file");
    return;
  }

  if(version_control_doc["version"] <= FIRMWARE_VERSION){
    Serial.println("No updates avaliable");
    return;
  }
  
  //wifi_client.setTimeout(30000);

  Serial.println("Update avaliable");
  Serial.println("Updating...");
  
  httpUpdate.rebootOnUpdate(false);

  /*Update.onProgress([](size_t progress, size_t total){
    Serial.print(progress * 100 / total);
    Serial.print(" ");
  });*/

  t_httpUpdate_return update_status = httpUpdate.update(wifi_client, version_control_doc["sketch_url"]);

  switch (update_status){
    case HTTP_UPDATE_FAILED:{
      String s = httpUpdate.getLastErrorString();
      Serial.println("\nUpdate error: " + s);
    } break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("\nNo update avaliable");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("\nUpdate succesfull, restarting...");
      ESP.restart();
    break;
  }
}


// Setup function
void setup() {
  Serial.begin(115200);
  if(!mics6814_init())
    Serial.println("Failed to start the sensor");
  wifi_init();
  mqtt_init();
}

// Loop function
void loop() {
  if(WiFi.status() != WL_CONNECTED)
    wifi_init();
  if (!mqtt_client.connected())
    mqtt_reconnect();
  mqtt_client.loop();
  mqtt_publish();
  ota_update();
  delay(1000 * 10); // Waits 10 seconds
}
