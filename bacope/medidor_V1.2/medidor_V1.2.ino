#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int oneWireBus = 2;
const int pulsadorPin = 3;  // Ajusta el número del pin del pulsador

DeviceAddress sensorFrio = { 0x28, 0x52, 0x1E, 0x28, 0x0, 0x0, 0x80, 0x60 };
DeviceAddress sensorCaliente = { 0x28, 0x68, 0x4, 0x28, 0x0, 0x0, 0x80, 0x83 };

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

const int led_verde = 8;
const int led_rojo = 10;
const int pinSensorCorriente = A1;  // Cambia el número del pin según tu configuración
const int led_amarillo = 9;
LiquidCrystal_I2C lcd(0x27, 20, 4);

boolean arranqueIniciado = false;
unsigned long tiempoInicio = 0;
const unsigned long tiempoEspera =60000;  // 2 minutos en milisegundos

void setup() {
  sensors.begin();
  pinMode(led_verde, OUTPUT);
  pinMode(led_rojo, OUTPUT);
  pinMode(led_amarillo, OUTPUT);
  pinMode(pinSensorCorriente, INPUT);
  pinMode(pulsadorPin, INPUT_PULLUP);  // Configura el pin del pulsador como entrada con resistencia pull-up
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  digitalWrite(led_amarillo, LOW);
}

void loop() {
  if (digitalRead(pulsadorPin) == LOW && !arranqueIniciado) {
    arranqueIniciado = true;
    tiempoInicio = millis();  // Guarda el tiempo actual

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Arrancando...");
    delay(1000);  // Puedes ajustar el tiempo de espera según tus necesidades

    while (arranqueIniciado) {
      unsigned long tiempoActual = millis();  // Obtiene el tiempo actual

        sensors.requestTemperatures();
        float tempFrio = sensors.getTempC(sensorFrio);
        float tempCaliente = sensors.getTempC(sensorCaliente);

        // Lectura de corriente usando el sensor ACS712
        int sensorValue = analogRead(pinSensorCorriente);
        float corriente = (sensorValue - 512) / 102.3;  // Ajuste según la sensibilidad del ACS712

        lcd.setCursor(9, 0);
        lcd.print("C: ");
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
          lcd.setCursor(1,1);
          lcd.print("Error 1");
          delay(10000);
          arranqueIniciado = false;
        }
        else if (tempCaliente < 70.0 && tempFrio > 10.0) {
         lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Error 3");
          lcd.setCursor(0,1);
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
