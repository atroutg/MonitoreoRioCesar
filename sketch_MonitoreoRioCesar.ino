// Sistema de monitoreo de Contaminacion en el rio Cesar
// Autor codigo: Armando Trout Garcia
// Universidad Abierta y a distancia - UNAD
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_PH 34
#define PIN_TURB 35
#define PIN_DS18B20 4
#define LED 18
#define BUZZER 19

LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensorTemp(&oneWire);

String mensaje = "  Sistema Automatizado para la Recoleccion y Almacenamiento de Contaminantes del Rio Cesar";
int scrollSpeed = 50; // Velocidad en milisegundos

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  sensorTemp.begin(); 
  
  // banner de Bienvenida
  lcd.init();  
  lcd.backlight();
  lcd.setCursor(16,2);  
  lcd.setCursor(0, 1);
  lcd.print(" "); //fila fija sin mensaje
  scrollFilaArriba(mensaje, scrollSpeed); //llama a funcion scroll
  lcd.clear();
}

void loop() {

  // leer sensores ph y turbidez
  int rawPH = analogRead(PIN_PH);
  int rawTurb = analogRead(PIN_TURB);  

  // Leer temperatura DS18B20
  sensorTemp.requestTemperatures();
  float temp = sensorTemp.getTempCByIndex(0); 

  float ph = map(rawPH, 0, 4095, 0, 140) / 10.0;
  int turb = map(rawTurb, 0, 4095, 0, 100);

  bool alerta = false;
  
  // rangos de evaluacion
  if (ph < 6.5 || ph > 9) alerta = true; 
  if (turb > 50) alerta = true;
  if (temp > 32) alerta = true;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("pH:");
  lcd.print(ph,1);
  lcd.print("  T:");
  lcd.print(temp,0);
  lcd.print("\337C"); 

  lcd.setCursor(0,1);
  lcd.print("Turb:");
  lcd.print(turb);

  if(alerta){
    digitalWrite(LED, HIGH);
    tone(BUZZER, 500, 300);
    lcd.print(" Alerta!");
     // Ejemplo de llamada de la función

    // enviarSMS(ph, turb, temp); //llamado a funcion de envio de SMS 
    // Simulación envio de SMS
    Serial.println("=== ALERTA CELULAR ===");
    Serial.println("Rio Cesar Contaminado!");
    Serial.print("pH: "); Serial.println(ph);
    Serial.print("Turbidez: "); Serial.println(turb);
    Serial.print("Temp: "); Serial.print(temp); Serial.println(" °C"); 
    Serial.println("SMS enviado");
    Serial.println("----------------");
  } else {
    digitalWrite(LED, LOW);
    noTone(BUZZER);
  }

  delay(2000);
}

//funcion scroll
void scrollFilaArriba(String texto, int velocidad) {
  // Añadimos espacios al final para que el texto salga de pantalla suavemente
  String temp = texto + "                ";
  
  for (int i = 0; i < temp.length() - 16; i++) {
    lcd.setCursor(0, 0);
    // Extrae una "ventana" de 16 caracteres y la imprime arriba
    lcd.print(temp.substring(i, i + 16));
    delay(velocidad);
  }
}

// Función para enviar alerta por SMS usando comandos AT
void enviarSMS(float ph, float turb, float temp) {
// Código propuesto como ejemplo teórico para envío de SMS mediante módem GSM.
// No ha sido probado físicamente con hardware real.
// Puede requerir ajustes según el módulo utilizado (SIM800L, SIM900, A6, etc.),
// velocidad baud, conexión serial y operador móvil.

  Serial.println("AT");                 
  delay(1000);

  Serial.println("AT+CMGF=1");         
  delay(1000);

  Serial.println("AT+CMGS=\"+573001234567\"");   // Cambiar número por el destinatario
  delay(1000);

  // Contenido del mensaje
  Serial.println("=== ALERTA CELULAR ===");
  Serial.println("Rio Cesar Contaminado!");

  Serial.print("pH: ");
  Serial.println(ph);

  Serial.print("Turbidez: ");
  Serial.println(turb);

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" C");

  // Envía SMS
  Serial.write(26);   // CTRL+Z
  delay(3000);

  Serial.println("SMS enviado");
  Serial.println("----------------");
}