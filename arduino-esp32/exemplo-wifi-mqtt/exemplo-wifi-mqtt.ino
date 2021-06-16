/*
 * Sketch exemplo contendo funções de conexão ao WiFi e ao broker MQTT
 * Obs..: Funcionando apenas na versão 1.0.4 da bilbioteca arduino-esp32, versões mais recentes impossibilitam conexão ao broker (erro -2) -> Todo: Descobrir o motivo
 */

// Bilbiotecas utilizadas
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Credenciais do WiFi
#define WIFI_SSID "Ribeiro_2.4G"
#define WIFI_PASSWORD "99955015"

// Endereço e porta do broker
#define BROKER_ADDRESS "test.mosquitto.org"
#define BROKER_PORT 8883

// Usuário e senha para autenticação no broker
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

// Tópicos para publicar e subescrever
#define PUBLISH_TOPIC "esp32/status"
#define SUBSCRIBE_TOPIC "esp32/subscribe"

// Id do ESP32 no broker, deve ser único -> Todo: gerar um id aleatório automaticamente
const char* client_id = "d45e956a-bd6b-4d94-ae01-026e1568e1a2";

// Inicia um cliente wifi seguro e um cliente mqtt
WiFiClientSecure wifi_client;
PubSubClient mqtt_client(wifi_client);

/*
 * A função mqtt_callback() é responsável por receber mensagens publicadas no tópico em que o ESP32 está inscrito
 * - Este exemplo apenas pega a mensagem recebida, sava em um string e o escreve no Serial Monitor
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length){
  String message;
  for(int i = 0; i < length; i++){
    message += (char)payload[i];
  }
  Serial.print("Mensagem recebida do broker: ");
  Serial.println(message);
}

/*
 * A função wifi_reconnect() é responsável por estabelecer a conexão do ESP32 com WiFi
 */
void wifi_reconnect(){
  // Conecta com WiFi
  Serial.print("Conectando-se a: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Inicia a conexão
  uint8_t counter = 0;
  while(WiFi.status() != WL_CONNECTED){ // Enquanto estiver no processo de conexão printa um *
    delay(200);
    Serial.print("*");
    counter++;
    if(counter > 50){ // Se o ESP32 ficar muito tempo tentando se conectar ao WiFi e não obter exito, o microcontrolador é reiniciado
      Serial.println(" Não foi possível se conectar a rede WiFi, reiniciando o ESP32...");
      delay(3000);
      ESP.restart();
    }
  }
  Serial.println(" Conectado com sucesso");
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP()); // Mostra o ip entregue ao ESP32 pelo DHCP
}

/*
 * A função mqtt_reconnect() é responsável por estabelecer a conexão do ESP32 com o broker MQTT
 */
void mqtt_reconnect(){
  // Conecta com o broker
  mqtt_client.setServer(BROKER_ADDRESS, BROKER_PORT); // Indica o endereço e a porta do broker que será conectado
  mqtt_client.setCallback(mqtt_callback); // Indica a função que irá cuidar de receber as mensagens do broker
  uint8_t counter = 0;
  while(!mqtt_client.connected()){
    Serial.print("Conectando ao broker... ");
    if(mqtt_client.connect(client_id, MQTT_USERNAME, MQTT_PASSWORD)){
      Serial.println("Conectado com sucesso");
      mqtt_client.subscribe(SUBSCRIBE_TOPIC); // Indica por qual tópico o ESP32 vai receber as mensagens do broker
      mqtt_client.publish(PUBLISH_TOPIC, "Conectado");  // Publica uma mensagem de conexão no broker
    }
    else{
      Serial.print("Falha em conectar ao broker. Codigo do erro obtido: ");
      Serial.println(mqtt_client.state());  // Mostra o código do erro que foi retornado. Mais informação consultar a documentação da biblioteca PubSubClient
      mqtt_client.disconnect();
      counter++;
      if(counter >= 5){ // Se o ESP32 ficar muito tempo tentando se conectar ao broker e não obter exito, o microcontrolador é reiniciado 
        ESP.restart();
      }
      delay(2000);
    }
  }
}

void setup(){
  Serial.begin(115200); // Velocidade de transmissão dos dados para porta serial
  
  // Conecta ao WiFi e ao broker MQTT
  wifi_reconnect();
  mqtt_reconnect();
}

void loop(){
  if(WiFi.status() != WL_CONNECTED) // Checa se o ESP32 se desconectou do WiFi, se sim, reconecta
    wifi_reconnect();
    
  if (!mqtt_client.connected()) // Checa se o ESP32 se desconectou do broker MQTT, se sim, reconecta
    mqtt_reconnect();
    
  mqtt_client.loop(); // Garante com que as mensagens sejam recebidas pelo ESP32
}
