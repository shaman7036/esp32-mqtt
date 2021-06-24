#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, 16);

void setup() {
  Serial.begin(115200);

  if(!Wire.begin(4, 15)){
    Serial.println("Falha ao inicializar I2C...");
    delay(1000);
    ESP.restart();
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  delay(2000);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello, world!");
  display.display();
}

void loop() {
  
}
