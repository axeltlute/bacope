#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Pin del bus OneWire
const int oneWireBus = 2;  // Cambia el número del pin según tu configuración

// Direcciones de los sensores DS18B20
DeviceAddress sensorFrio = { 0x28, 0x52, 0x1E, 0x28, 0x0, 0x0, 0x80, 0x60 };
DeviceAddress sensorCaliente = { 0x28, 0x68, 0x4, 0x28, 0x0, 0x0, 0x80, 0x83 };

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// Pines de los LEDs
const int led_verde = 8;  // Cambia el número del pin según tu configuración
const int led_rojo = 10;  // Cambia el número del pin según tu configuración
void setup() {

  sensors.begin();
  pinMode(led_verde, OUTPUT);
  pinMode(led_rojo, OUTPUT);
  lcd.init();  // initialize the lcd
  lcd.init();
  lcd.backlight();
}

void loop() {
  sensors.requestTemperatures();

  float tempFrio = sensors.getTempC(sensorFrio);
  float tempCaliente = sensors.getTempC(sensorCaliente);
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


  // Verificar las condiciones de temperatura y encender los LEDs correspondientes
  if (tempFrio <= 10.0 && tempCaliente >= 70.0) {
    digitalWrite(led_rojo, LOW);    // Encender el otro LED
    digitalWrite(led_verde, HIGH);  // Encender LED
    lcd.setCursor(0, 0);
    lcd.print("maquina aprobada");

  } else if (tempCaliente >= 70.0 && tempFrio > 10.0) {
    digitalWrite(led_rojo, HIGH);  // Encender el otro LED
    digitalWrite(led_verde, LOW);  // Encender el otro LED
    lcd.setCursor(0, 0);
    lcd.print("                  ");
    lcd.setCursor(0, 0);
    lcd.print("error2");
  } else if (tempCaliente <= 70.0 && tempFrio <= 10.0) {
    digitalWrite(led_rojo, HIGH);  // Encender el otro LED
    lcd.setCursor(0, 0);
    lcd.print("TF: ");
    lcd.setCursor(3, 0);
    lcd.print(tempFrio);
    lcd.setCursor(7, 0);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("            ");
    lcd.setCursor(0, 1);
    lcd.print("error1");
  }
  delay(1000);  // Pausa de 1 segundo entre lecturas
}
