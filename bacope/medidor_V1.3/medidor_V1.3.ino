#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>

const int oneWireBus = 2;
const int pulsadorPin = 3;

DeviceAddress sensorFrio = { 0x28, 0x52, 0x1E, 0x28, 0x0, 0x0, 0x80, 0x60 };
DeviceAddress sensorCaliente = { 0x28, 0x68, 0x4, 0x28, 0x0, 0x0, 0x80, 0x83 };

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int led_verde = 8;
const int led_rojo = 10;
const int pinSensorCorriente = A1;
const int led_amarillo = 9;
LiquidCrystal_I2C lcd(0x27, 20, 4);

File dataFile;
const char *fileName = "data.txt";

boolean arranqueIniciado = false;
unsigned long tiempoInicio = 0;
const unsigned long tiempoEspera = 60000;

void setup() {
  sensors.begin();
  pinMode(led_verde, OUTPUT);
  pinMode(led_rojo, OUTPUT);
  pinMode(led_amarillo, OUTPUT);
  pinMode(pinSensorCorriente, INPUT);
  pinMode(pulsadorPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  digitalWrite(led_amarillo, LOW);

  // Inicializa la tarjeta SD
  if (SD.begin()) {
    Serial.println("Tarjeta SD inicializada correctamente.");
  } else {
    lcd.println("Error4");
  }
}

void loop() {
  if (digitalRead(pulsadorPin) == LOW && !arranqueIniciado) {
    arranqueIniciado = true;
    tiempoInicio = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Arrancando...");
    delay(1000);

    // Abre el archivo en modo de escritura
    dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println("Tiempo,TempFrio,TempCaliente,Corriente");
      dataFile.close();
    } else {
      lcd.println("Error4");
    }

    while (arranqueIniciado) {
      unsigned long tiempoActual = millis();

      sensors.requestTemperatures();
      float tempFrio = sensors.getTempC(sensorFrio);
      float tempCaliente = sensors.getTempC(sensorCaliente);
      int sensorValue = analogRead(pinSensorCorriente);
      float corriente = (sensorValue - 512) / 102.3;

      // Abre el archivo en modo de escritura y agrega datos
      dataFile = SD.open(fileName, FILE_WRITE);
      if (dataFile) {
        dataFile.print(tiempoActual - tiempoInicio);
        dataFile.print(",");
        dataFile.print(tempFrio);
        dataFile.print(",");
        dataFile.print(tempCaliente);
        dataFile.print(",");
        dataFile.println(corriente);
        dataFile.close();
      } else {
        lcd.println("Error4");
      }

      lcd.setCursor(9, 0);
      lcd.print("A: ");
      lcd.print(corriente);

      lcd.setCursor(0, 0);
      lcd.print("TF: ");
      lcd.setCursor(3, 0);
      lcd.print(tempFrio);
      lcd.setCursor(7, 0);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("TC: ");
      lcd.setCursor(3, 1);
      lcd.print(tempCaliente);
      lcd.setCursor(7, 1);
      lcd.println("C");
      lcd.setCursor(8, 1);
      lcd.println("        ");

      if (tiempoActual - tiempoInicio >= tiempoEspera) {
        if (tempFrio <= 10.0 && tempCaliente >= 70.0) {
          digitalWrite(led_rojo, LOW);
          digitalWrite(led_verde, HIGH);
          digitalWrite(led_amarillo, LOW);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Maquina  ");
          lcd.setCursor(0, 1);
          lcd.print("aprobada");
          delay(10000);
          arranqueIniciado = false;
        } else if (tempCaliente >= 70.0 && tempFrio > 10.0) {
          digitalWrite(led_rojo, HIGH);
          digitalWrite(led_verde, LOW);
          digitalWrite(led_amarillo, LOW);
          lcd.setCursor(0, 0);
          lcd.print("Error 2");
          delay(10000);
          arranqueIniciado = false;
        } else if (tempCaliente <= 70.0 && tempFrio <= 10.0) {
          digitalWrite(led_rojo, HIGH);
          digitalWrite(led_verde, LOW);
          digitalWrite(led_amarillo, LOW);
          lcd.setCursor(1, 1);
          lcd.print("Error 1");
          delay(10000);
          arranqueIniciado = false;
        } else if (tempCaliente < 70.0 && tempFrio > 10.0) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Error 3");
          lcd.setCursor(0, 1);
          lcd.print("volver a medir");
          delay(10000);
          arranqueIniciado = false;
        }
        Serial.println(corriente);  // Muestra la corriente en la consola serial
        delay(5000);
        arranqueIniciado = false;
      }
    }
  } else {
    // Si el pulsador no está presionado, muestra el mensaje en la pantalla LCD
    arranqueIniciado = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Presione para");
    lcd.setCursor(0, 1);
    lcd.print("iniciar");
    delay(100);
    digitalWrite(led_amarillo, HIGH);
    digitalWrite(led_rojo, LOW);
    digitalWrite(led_verde, LOW);
  }
}
