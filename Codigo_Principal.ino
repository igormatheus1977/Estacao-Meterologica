#include <LiquidCrystal_I2C.h>                // Biblioteca do LCD (Display)
#include <Adafruit_Sensor.h>                  // Biblioteca base de sensores do Adafruit -- Necessário para o BMP280
#include <Adafruit_BMP280.h>                  // Biblioteca do sensor de pressão (BMP280)
#include <DHT.h>                              // Biblioteca do sensor de umidade (DHT22)


// Definição de microcontroladores

Adafruit_BMP280 bmp;                          // Sensor de pressão
LiquidCrystal_I2C lcd(0x27,16,2);             // LCD definido no endereço hexadecimal 0x27 (Explicação abaixo)
DHT dht(2, DHT22);                            // Sensor de umidade

/* 
Explicação para a definição do endereço do LCD:
- O LCD e o BMP280 precisam estar conectados ao SDA e ao SCL, então podem dividir os mesmos fios
- Definições nesse projeto:
    LCD em 0x27
    BMP280 em 0x76
Mais abaixo está a linha do código para o BMP280
*/


float pressao, alt, temp, tensao, temp_ajustada, temp_bin, umid, umid_ajustada, lum_bin, lum, VLDR, RLDR;
float inv_a_temp = 1.002226;
float inv_b_temp = -0.5652;
float inv_a_umid = 1.481384;
float inv_b_umid = -25.527471;


// Definição de caracteres especiais do LDC

byte graus[] = {
  B11100,
  B10100,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte cedilha[] = {
  B00000,
  B00000,
  B01110,
  B10000,
  B10001,
  B01110,
  B00100,
  B01100
};

byte aTil[] = {
  B01101,
  B10010,
  B00000,
  B01110,
  B00001,
  B01111,
  B10001,
  B01111
};

void setup() {

  Serial.begin(9600);                         // Inicia comunicação serial a 9600 bps de baud rate

  lcd.init();                                 // Liga o LCD

  // Guarda caracteres especiais nos slots do LCD
  lcd.createChar(0, graus);
  lcd.createChar(1, cedilha);
  lcd.createChar(2, aTil);

  lcd.backlight();                            // Liga o backlight (Luz mais forte do LCD)
  bmp.begin(0x76);                            // Inicia o BMP280 (sensor de pressão) no endereço hexadecimal 0x76
  dht.begin();                                // Inicia o DHT22 (sensor de umidade)

  lcd.setCursor(0,0);                         // Põe o cursor em (coluna, linha)
  lcd.print("InstruMed");
  lcd.setCursor(0,1);  
  lcd.print("Esta");
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.print("o Meteo.");

  delay(2000);
  lcd.clear();                                // Limpa o visor do LCD

  lcd.setCursor(0,0);
  lcd.print("Alunos:");
  lcd.setCursor(0,1);
  lcd.print("Mateus   ");
  delay(1000);

  lcd.setCursor(0,1);
  lcd.print("Igor     ");
  delay(1000);

  lcd.setCursor(0,1);
  lcd.print("Eduardo  ");
  delay(1000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Grupo:");
  lcd.setCursor(0,1);
  lcd.print("Macacada");
  delay(2500);
  lcd.clear();

  
}

void loop() {

  /* 
  -- ANALOG REFERENCE -- 
  Define tensão de referência
  Default - Padrão (5 V ou 3,3 V)
  Internal - Baixa tensão para maior precisão (1,1 V)
  External - Tensão aplicada no pino AREF (não utilizada nesse projeto)
  */
  

  // Sensor de luminosidade (LDR)
  analogReference(DEFAULT);                                 // 3,3 V
  analogRead(A1);                                           // Leitura descartável para estabilizar o ADC
  delay(10);
  lum_bin = analogRead(A1);                                 // Leitura em binário
  VLDR = lum_bin*5.0/1024;                                  // Binário -> Tensão (V)
  RLDR = VLDR/(3.3 - VLDR)*100000;                          // Tensão -> Resistência (Ohm)
  lum = pow(10,-1.664902*log10(RLDR) + 7.723045);           // Resistência -> Luminância (lx)

  // Sensor de temperatura (LM35)
  analogReference(INTERNAL);                                // 1,1 V
  analogRead(A0);                                           // Leitura descartável para estabilizar o ADC
  delay(10);
  temp_bin = analogRead(A0);                                // Leitura em binário
  temp = (temp_bin*(1.1/1024))*100;                         // Binário -> Temperatura (°C)
  temp_ajustada = temp * inv_a_temp + inv_b_temp;           // Ajuste da temperatura

  // Sensor de pressão e temperatura (BMP280)
  pressao = bmp.readPressure();                             // Leitura de pressão (Pa)
  alt = bmp.readAltitude(1013.25);                          // Leitura de altitude com base na pressão ao nível do mar

  // Sensor de umidade (DHT22)
  umid = dht.readHumidity();                                // Leitura de umidade
  umid_ajustada = umid*inv_a_umid + inv_b_umid;             // Ajuste da umidade

  // LEDS
  pinMode(4, OUTPUT);                                       // Define o que acontece no pino (saída de tensão)
  digitalWrite(4, HIGH);                                    // Acende o pino D4

  // Botão
  pinMode(3, INPUT);

  lcd.setCursor(0,0);
  lcd.print("Temperatura:");
  lcd.setCursor(0,1);
  lcd.print(temp_ajustada, 2);
  lcd.print(" ");
  lcd.write(byte(0));
  lcd.print("C");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Umidade:");
  lcd.setCursor(0,1);
  lcd.print(umid_ajustada, 1);
  lcd.print(" %");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Press");
  lcd.write(byte(2));
  lcd.print("o:");
  lcd.setCursor(0,1);  
  lcd.print(pressao, 2);
  lcd.print(" ");
  lcd.print("Pa");  
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Altitude:");
  lcd.setCursor(0,1);
  lcd.print(alt);
  lcd.print(" ");
  lcd.print("m");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Luminosidade:");
  lcd.setCursor(0,1);
  lcd.print(lum);
  lcd.print(" lx");
  delay(2000);
  lcd.clear();

}