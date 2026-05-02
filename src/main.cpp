/***************************************************************
 UNIVERSIDAD NACIONAL ABIERTA Y A DISTANCIA - UNAD - 2026
 Programa de Ingeniería Electrónica
 Microprocesadores y Microcontroladores
 GRUPO 309696_1
 Fase 3 – Diseño y Desarrollo de la Solución al Problema Planteado
 "Sistema Embebido Inteligente para Monitoreo Ambiental y 
 Recolección Automatizada de Residuos en el Río Cesar"

 INTEGRANTES:
 ARMANDO GUILLERMO TROUT GARCIA 
 JORDAN JAVIER RADA CARDONA
 LAINIKER EDUARDO YANEZ GUTIERREZ 
 SEBASTIAN MACHADO PERALTA

 TUTOR:
 Ing.  JULIO CESAR VIDAL

 RANGOS DE EVALUACION: 
 - pH: 6.5 - 9.0 
 - Turbidez: < 50 NTU 
 - Temperatura: ≤ 32 °C

 ESP32 + LCD I2C + Sensores + LEDs + Buzzer + GSM + Relé + Pulsador
***************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//------------------------------------------------------
// Configuración del LCD
//------------------------------------------------------
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_ROWS    2
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

//------------------------------------------------------
// DS18B20
//------------------------------------------------------
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//------------------------------------------------------
// PINES
//------------------------------------------------------
#define PIN_PH         34
#define PIN_TURBIDEZ   35

#define LED_PH_VERDE   25
#define LED_PH_ROJO    26

#define LED_TURB_VERDE 33
#define LED_TURB_ROJO  27

#define LED_TEMP_VERDE 32
#define LED_TEMP_ROJO  14

#define BUZZER         19
#define RELE           18
#define PULSADOR       15

//------------------------------------------------------
// VARIABLES
//------------------------------------------------------
float ph = 0;
float turbidez = 0;
float temperatura = 0;

bool alertaPH = false;
bool alertaTurb = false;
bool alertaTemp = false;
bool alertaGeneral = false;

bool smsEnviado = false;

//------------------------------------------------------
// TEMPORIZADORES
//------------------------------------------------------
unsigned long tSensores = 0;
unsigned long tBuzzer = 0;

//------------------------------------------------------
// SCROLL
//------------------------------------------------------
String textoLargo = ""; // Variable para almacenar el texto completo
unsigned long tiempoAnteriorScroll = 0;
const long intervaloScroll = 500; // Intervalo de desplazamiento (ms)
int posicionActual=0;

//------------------------------------------------------
// SENSORES SECUENCIALES
//------------------------------------------------------

unsigned long lastDisplayTime = 0;
int currentSensor = 0;
const int displayInterval = 200; // Tiempo en milisegundos 

// Arreglo con los mensajes de los sensores
String sensorMessages[3];

//------------------------------------------------------
// BUZZER
//------------------------------------------------------
bool estadoBuzzer = false;

//------------------------------------------------------
// INTERRUPCION PULSADOR
//------------------------------------------------------
volatile bool estadoRutina = false;  // Estado del toggle (relé/buzzer)
volatile unsigned long tiempoUltimoFlanco = 0; // Tiempo del último flanco
const unsigned long tiempoDebounce = 50; // Tiempo de debounce (50 ms)

//------------------------------------------------------
// SERVICIO DE INTERRUPCIÓN PARA EL PULSADOR
//------------------------------------------------------

void IRAM_ATTR isrPulsador() {
  unsigned long tiempoActual = millis();

  // Debounce: Ignorar cambios si no ha pasado suficiente tiempo
  if ((tiempoActual - tiempoUltimoFlanco) > tiempoDebounce) {
    estadoRutina = !estadoRutina; // Alternar el estado    
    tiempoUltimoFlanco = tiempoActual; // Actualizar el tiempo del último flanco
     
  }
}

//------------------------------------------------------
// LECTURAS DE SENSORES
//------------------------------------------------------
float leerPH() {
   // Convertir a escala de pH (0-14)
  return (analogRead(PIN_PH) / 4095.0) * 14.0;
}

float leerTurbidez() {
  // Convertir a porcentaje de turbidez (0-100%)
  return (analogRead(PIN_TURBIDEZ) / 4095.0) * 100.0; 
}

float leerTemperatura() {
  // Solicitar lectura de temperatura
  sensors.requestTemperatures(); 
  // Obtener temperatura en °C del sensor conectado
  return sensors.getTempCByIndex(0); 
}

//------------------------------------------------------
// EVALUACION DE RANGOS DE SENSORES
//------------------------------------------------------
void evaluarPH() {
  if (ph >= 6.5 && ph <= 9.0) {
    digitalWrite(LED_PH_VERDE, HIGH);
    digitalWrite(LED_PH_ROJO, LOW);
    alertaPH = false; 
  } else {
    digitalWrite(LED_PH_VERDE, LOW);
    digitalWrite(LED_PH_ROJO, HIGH);
    alertaPH = true;
  }
}

void evaluarTurbidez() {
  if (turbidez < 50) {
    digitalWrite(LED_TURB_VERDE, HIGH);
    digitalWrite(LED_TURB_ROJO, LOW);
    alertaTurb = false;
  } else {
    digitalWrite(LED_TURB_VERDE, LOW);
    digitalWrite(LED_TURB_ROJO, HIGH);
    alertaTurb = true;
  }
}

void evaluarTemperatura() {
  if (temperatura <= 32) {
    digitalWrite(LED_TEMP_VERDE, HIGH);
    digitalWrite(LED_TEMP_ROJO, LOW);
    alertaTemp = false;
  } else {
    digitalWrite(LED_TEMP_VERDE, LOW);
    digitalWrite(LED_TEMP_ROJO, HIGH);
    alertaTemp = true;
  }
}

void evaluarGeneral() {
  alertaGeneral = alertaPH || alertaTurb || alertaTemp;
}




void mostrarSensoresSecuencialmente() {
  String estadoSensor ="";  
  
  // Actualizar los mensajes de los sensores
  sensorMessages[0] = "pH:" + String(ph, 1);
  sensorMessages[1] = "Turbidez:" + String(turbidez, 0) + " NTU";
  sensorMessages[2] = "Temperatura:" + String(temperatura, 0) + (char)223 + "C"; // 223 es el código ASCII para el símbolo de grado
      
  lcd.setCursor(0, 0); 
  if (!alertaGeneral) {
    lcd.print("  Rio Estable  ");  

  } else {
    lcd.print("Rio Contaminado");
    if (alertaPH)   sensorMessages[0] ="*pH:" + String(ph, 1);
    if (alertaTurb) sensorMessages[1] ="*Turbidez:" + String(turbidez, 0) + " NTU";
    if (alertaTemp) sensorMessages[2] ="*Temperatura:" + String(temperatura, 0) + (char)223 + "C";
  }

 
  // Mostrar el sensor actual en la segunda fila
  unsigned long currentTime = millis();
  if (currentTime - lastDisplayTime >= displayInterval) {
    lastDisplayTime = currentTime;

    // Limpiar la segunda fila
    lcd.setCursor(0, 1);
    lcd.print("                "); // 16 espacios en blanco

    // Mostrar el sensor actual
    lcd.setCursor(0, 1);
    lcd.print(sensorMessages[currentSensor]);

    // Avanzar al siguiente sensor
    currentSensor = (currentSensor + 1) % 3; // Cambia entre 0, 1 y 2
  }
}

//------------------------------------------------------
// PREPARAR TEXTO PARA SCROLL
//------------------------------------------------------
void prepararScroll() {
  String msg = "";

  if (!alertaGeneral) {
    lcd.setCursor(0, 0);
    lcd.print("  Rio Estable  ");
    lcd.setCursor(0, 1);
     lcd.print("                "); // 16 espacios en blanco    
    msg = " pH:" + String(ph, 1) + " Turb:" + String(turbidez, 0) + "NTU" + " Temp:" + String(temperatura, 0) + "\337C";
    msg += "        "; // Rellenar con espacios para scroll suave
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Rio Contaminado");
    lcd.setCursor(0, 1); 
     lcd.print("                "); // 16 espacios en blanco   
    if (alertaPH)   msg += "*pH:" + String(ph, 1) + " ";
    if (alertaTurb) msg += "*Turb:" + String(turbidez, 0) + "NTU ";
    if (alertaTemp) msg += "*Temp:" + String(temperatura, 0) + "\337C";    
  }
  textoLargo = msg; // Guardar el texto completo para el scroll    

}


//------------------------------------------------------
// SCROLL TEXTO
//------------------------------------------------------
void scrollTexto() {
  unsigned long tiempoActual = millis(); // Obtener el tiempo actual
  lcd.setCursor(0, 1);  // Posicionar el cursor al inicio de la segunda fila     

       if(textoLargo.length() <= 16) {
          lcd.print("                "); // Borrar la línea (16 espacios)
          lcd.setCursor(0, 1);
          lcd.print(textoLargo);                  
       }else{
                
                // Si ha pasado el intervalo de tiempo
                if (tiempoActual - tiempoAnteriorScroll >= 150) {
                    
                    lcd.print("                "); // 16 espacios para borrar                
                    lcd.setCursor(0, 1);
                    // Muestra el substring actual                    
                    lcd.print(textoLargo.substring(posicionActual, posicionActual + 16));
                    tiempoAnteriorScroll = tiempoActual; // Actualiza el tiempo anterior 

                    // Avanza la posición
                    posicionActual++ ;

                    // Reinicia si llega al final del texto
                    if (posicionActual >= textoLargo.length() - 15) {
                       posicionActual = 0;
                    
                    }
                } 
          }      
  }



//------------------------------------------------------
// BUZZER INTERMITENTE
//------------------------------------------------------
void controlarBuzzer() {
  // Si hay una alerta general, hacer que el buzzer suene intermitentemente
  if (alertaGeneral) {
    if (millis() - tBuzzer >= 500) {
      tBuzzer = millis();
      estadoBuzzer = !estadoBuzzer; // Alternar estado del buzzer
      if (estadoBuzzer) {
         tone(BUZZER, 500); // Generar un tono de 500Hz
      } else {
         noTone(BUZZER);     // Detener el tono
      }
    }
  } else {
    noTone(BUZZER);     // Detener el tono
    estadoBuzzer = false;
  }
}

//------------------------------------------------------
// SMS SIMULADO
//------------------------------------------------------
void enviarSMS() {
  // comandos AT para enviar SMS (simulado en consola)
  Serial.println("===== MODEM GSM =====");
  Serial.println("AT");
  Serial.println("OK");
  Serial.println("AT+CMGF=1");
  Serial.println("OK");
  Serial.println("AT+CMGS=\"3001234567\"");
  Serial.println("ALERTA RIO CESAR");
  // Enviar detalles de la alerta
  // con el sensor que haya generado la alerta y su valor actual
  if (alertaPH) Serial.println("pH fuera rango");
  if (alertaTurb) Serial.println("Alta turbidez");
  if (alertaTemp) Serial.println("Alta temperatura");

  Serial.println("Agua contaminada");
  Serial.println((char)26);
  Serial.println("SMS ENVIADO");
}

//------------------------------------------------------
// SETUP
//------------------------------------------------------
void setup() {
  Serial.begin(115200);  
  // Configurar pines 
  pinMode(BUZZER, OUTPUT);
  pinMode(RELE, OUTPUT);
  pinMode(LED_PH_VERDE, OUTPUT);
  pinMode(LED_PH_ROJO, OUTPUT);
  pinMode(LED_TURB_VERDE, OUTPUT);
  pinMode(LED_TURB_ROJO, OUTPUT);
  pinMode(LED_TEMP_VERDE, OUTPUT);
  pinMode(LED_TEMP_ROJO, OUTPUT);
  pinMode(PULSADOR, INPUT_PULLUP);

  // Configurar la interrupción en el GPIO 15
  attachInterrupt(digitalPinToInterrupt(PULSADOR), isrPulsador, FALLING);  

  sensors.begin(); // Iniciar sensor de temperatura

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Monitoreo Rio");
  lcd.setCursor(0, 1);
  lcd.print("Cesar          "); // Rellenar con espacios  

  
  //Valores iniciales de los sensores
  ph = leerPH();
  turbidez = leerTurbidez();
  temperatura = leerTemperatura();

  // Preparar el texto para el scroll
  prepararScroll(); 
   
  // Prueba del buzzer al inicio
  tone(BUZZER, 500);
  delay(200);
  noTone(BUZZER);
  delay(1500); // Mostrar mensaje inicial por 2 segundos
  lcd.setCursor(0, 1);
  lcd.print("          "); // Limpiar la línea

}

//------------------------------------------------------
// LOOP
//------------------------------------------------------
void loop() {
  //--------------------------------------------------
  // Evalua Sensores cada segundo
  //--------------------------------------------------
  if (millis() - tSensores >= 1000) {
    tSensores = millis();
    ph = leerPH();
    turbidez = leerTurbidez();
    temperatura = leerTemperatura();

    evaluarPH();
    evaluarTurbidez();
    evaluarTemperatura();
    evaluarGeneral(); // Evaluar estado general después de evaluar cada parámetro

    prepararScroll();

    if(alertaGeneral) {
      digitalWrite(RELE, HIGH); // Activar el relé (HIGH)
    }

    if (alertaGeneral && !smsEnviado) {
      enviarSMS(); // Simulación de envío de SMS
      smsEnviado = true;
    }

    if (!alertaGeneral) {
      smsEnviado = false;
      digitalWrite(RELE, LOW); // Desactivar el relé (LOW)
    }
  }

  //--------------------------------------------------
  // Procesos continuos no bloqueantes
  //--------------------------------------------------ç
  // seleccionar modo de visualización: scroll o secuencial 
  // por defecto muestra secuencialmente los sensores
   if (estadoRutina) {
     scrollTexto();
    
  } else {
      mostrarSensoresSecuencialmente(); 
  }
  
  
  controlarBuzzer();
}