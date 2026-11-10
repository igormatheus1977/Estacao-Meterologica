#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h>
#include <DHT.h>

Adafruit_BMP280 bmp; 
LiquidCrystal_I2C lcd(0x27,16,2);
DHT dht(2, DHT22);


float pressao, alt, temp, tensao, temp_ajustada, temp_bin, umid, umid_ajustada, lum_bin, lum, VLDR, RLDR;
float inv_a_temp = 1.002226;
float inv_b_temp = -0.5652;
float inv_a_umid = 1.481384;
float inv_b_umid = -25.527471;

// caractere customizado de "°"
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
  Serial.begin(9600); 

  lcd.init(); 
  lcd.createChar(0, graus);
  lcd.createChar(1, cedilha);
  lcd.createChar(2, aTil);
  lcd.backlight();
  bmp.begin(0x76);
  dht.begin();

  lcd.setCursor(0,0);
  lcd.print("InstruMed");
  lcd.setCursor(0,1);  
  lcd.print("Esta");
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.print("o Meteo.");

  delay(2000);
  lcd.clear();

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

  
  analogReference(DEFAULT);
  analogRead(A1); 
  delay(10);
  lum_bin = analogRead(A1);
  VLDR = lum_bin*5.0/1024;
  RLDR = VLDR/(3.3 - VLDR)*100000; 
  lum = pow(10,-1.664902*log10(RLDR) + 7.723045);

  analogReference(INTERNAL);
  analogRead(A0);             
  delay(10);
  temp_bin = analogRead(A0);
  temp = (temp_bin*(1.1/1024))*100;  

  temp_ajustada = temp * inv_a_temp + inv_b_temp;

  pressao = bmp.readPressure();
  alt = bmp.readAltitude(1013.25);

  umid = dht.readHumidity();
  umid_ajustada = umid*inv_a_umid + inv_b_umid;

  // LED
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

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