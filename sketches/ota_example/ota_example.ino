#include <WiFi.h>
#include <HTTPUpdate.h>

const char* SSID = "Ribeiro_2.4G";
const char* PASSWORD = "99955015";

void setup() {
  Serial.begin(115200);

  WiFi.begin(SSID, PASSWORD);
  Serial.println("Conectando-se a: " + String(SSID));
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  Serial.println("Atualizando Sketch...");

  Update.onProgress([](size_t progresso, size_t total) {
    Serial.print(progresso * 100 / total);
    Serial.print(" ");
  });
  
  httpUpdate.rebootOnUpdate(false);

  WiFiClientSecure client;

  t_httpUpdate_return resultado = httpUpdate.update(client, "https://internetecoisas.com.br/download/IeC115-OTA/Blink.ino.esp32.bin");

  switch (resultado) {
    case HTTP_UPDATE_FAILED: {
      String s = httpUpdate.getLastErrorString();
      Serial.println("\nFalha: " + s);
    } break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("\nNenhuma atualização disponível");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("\nSucesso, reiniciando...");
      ESP.restart();
    break;
  }
}

void loop() {
}
