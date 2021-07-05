/*
 * Sketch exemplo de atualização por ota
 * Obs..: Funcionando apenas na versão 1.0.4 da bilbioteca arduino-esp32, versões mais recentes impossibilitam realizar o acesso ao arquivo de versionamento -> Todo: Descobrir o motivo
 */

#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPUpdate.h>

#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."

#define FIRMWARE_VERSION 1

WiFiClientSecure wifi_client;

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
 * A função ota_update() é responsável puxar um arquivo de versionamento contendo a versão mais recente e o link do sketch, se o sketch estiver desatualizado realizada a atualização com o link contido no arquivo
 */
void ota_update(){
  Serial.println("Verificando se existem atualizações disponíveis...");

  //mqtt_client.disconnect(); // Se usar mqtt deve-se desconectar do broker para não gerar erros no processo de atualização

  HTTPClient http_client; // Novo client http
  http_client.begin(wifi_client, "https://dl.dropboxusercontent.com/s/eeu9vhz48pe7l1q/version_control_file.txt");
  
  int httpCode = http_client.GET();  // Debug
  String s = http_client.getString();  //
  http_client.end(); //
  s.trim(); //
  
  if (httpCode != HTTP_CODE_OK) {  // Se não foi possível acessar o arquivo de controle de versão mostra um erro
    //log("ERRO HTTP " + String(httpCode) + " " + http_client.errorToString(httpCode));
    Serial.print("Falha ao verificar por atualizações -> ");
    Serial.print(String(httpCode));
    Serial.print(": ");
    Serial.println(http_client.errorToString(httpCode));
    return;
  }
  
  String version_control_file = http_client.getString();  // Salva o arquivo JSON de controle de versão em uma String
  http_client.end();  // Fecha a conexão http
  version_control_file.trim();  // Retira os espaços do arquivo

  StaticJsonDocument<200> version_control_doc;  // Inicia um documento json para deserializar o arquivo de controle de versão

  if (deserializeJson(version_control_doc, version_control_file)){  // Se não foi possível realizar a deserialização, mostra um erro
    Serial.println("Arquivo de controle de versão inválido");
    return;
  }

  if(version_control_doc["version"] <= FIRMWARE_VERSION){ // Se a versão indicada no arquivo for menor ou igual a versão atual do sketch (constante FIRMWARE_VERSION) não atualiza
    Serial.println("Não foram encontradas atualizações");
    return;
  }

  Serial.println("Nova versão disponível");
  Serial.println("Atualizando...");
  
  httpUpdate.rebootOnUpdate(false); // Desativa a reinicialização automática do esp32 após atualização

  t_httpUpdate_return update_status = httpUpdate.update(wifi_client, version_control_doc["sketch_url"]);  // Atualiza o sketch de acordo com o link contido no arquivo de versionamento

  switch (update_status){ // Verifica o estado da atualização
    case HTTP_UPDATE_FAILED:{ // Falhou na atualização
      String s = httpUpdate.getLastErrorString();
      Serial.println("\nErro ao atualizar: " + s);
    } break;
    case HTTP_UPDATE_NO_UPDATES:  // Não foram feitas atualizações
      Serial.println("\nAtualização não foi realizada com sucesso");
      break;
    case HTTP_UPDATE_OK:  // Atualizou com sucesso
      Serial.println("\nAtualizado com sucesso, reiniciando...");
      ESP.restart();
    break;
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED)
    wifi_reconnect();
  ota_update();
  delay(60 * 1000);
}
