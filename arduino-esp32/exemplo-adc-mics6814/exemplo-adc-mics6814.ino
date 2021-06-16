/*
 * Sketch exemplo contendo funções para leitura do conversor analógico digital
 * Obs..: Valor cru do ADC, preciso fazer as conversões para cada sensor utilizado -> Todo: Arrumar veituras em ppm do sensor de gás
 */

// Bilbiotecas utilizadas
#include <Arduino.h>
#include <driver/adc.h>

// Parametros para ler o ADC
#define ADC_MIN 0 // Valor com 0V
#define ADC_MAX 4095  // Valor com 3,9V
#define SAMPLES 64 // São realizadas 64 leituras consecutivas (tentar aumentar um pouco da precisão de leitura)

/*
 * A funcção mics6814_init() inicializa os canais ADC onde o sensor está conectado
 */
bool mics6814_init(){
  adc1_config_width(ADC_WIDTH_BIT_12);  // Configura o ADC para funcionar com largura de landa de 12 bits
  // Configura a atenuação de cada canal (11dB de 0V até 3.9V) -> Checar documentação do ADC do ESP32
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
  return true;
}

/*
 * A funcção mics6814_read() realiza 64 (ou a quantidade setada em SAMPLES de leituras no ADC e tira a média)
 */
bool mics6814_read(uint16_t* no2, uint16_t* nh3, uint16_t* co){
  uint16_t read_value;

  for(int i = 0; i < SAMPLES; i++){
    read_value = adc1_get_raw(ADC1_CHANNEL_0);
    if(read_value >= ADC_MIN && read_value <= ADC_MAX)
      *no2 += (uint16_t)read_value;
    else
      return false;
    read_value = adc1_get_raw(ADC1_CHANNEL_2);
    if(read_value >= ADC_MIN && read_value <= ADC_MAX)
      *nh3 += (uint16_t)read_value;
    else
      return false;
    read_value = adc1_get_raw(ADC1_CHANNEL_3);
    if(read_value >= ADC_MIN && read_value <= ADC_MAX)
      *co += (uint16_t)read_value;
    else
      return false; 
  }
  
  *no2 = *no2/SAMPLES;
  *nh3 = *nh3/SAMPLES;
  *co = *co/SAMPLES;
  
  return true;
}

void setup(){
  Serial.begin(115200);
  
  if(!mics6814_init())
    Serial.println("Falha ao iniciar o sensor");
}

void loop(){
  uint16_t no2 = 0;
  uint16_t nh3 = 0;
  uint16_t co = 0;

  if(mics6814_read(&no2, &nh3, &co)){
    Serial.print("NO2: ");
    Serial.println(no2);
    Serial.print("NH3: ");
    Serial.println(nh3);
    Serial.print("CO: ");
    Serial.println(co);
    Serial.println("-----------------------------------");
  }

  delay(1000 * 5);
}
