SISTEMA EMBEBIDO INTELIGENTE PARA MONITOREO DEL RÍO CESAR

UNIVERSIDAD: Universidad Nacional Abierta y a Distancia (UNAD) 
PROGRAMA: Ingeniería Electrónica 
ASIGNATURA: Microprocesadores y Microcontroladores 
GRUPO: 309696_1 
FASE: Fase 3 - Diseño y Desarrollo de la Solución

DESCRIPCIÓN: 
Este proyecto implementa un sistema embebido basado en
ESP32 para el monitoreo ambiental del Río Cesar. Permite medir pH,
turbidez y temperatura, mostrando los datos en un LCD y generando
alertas visuales, sonoras y digitales en caso de contaminación.

INTEGRANTES:
- Armando Guillermo Trout García
- Jordan Javier Rada Cardona
- Lainiker Eduardo Yanez Gutierrez
- Sebastián Machado Peralta

TUTOR:
- Ing. JULIO CESAR VIDAL

COMPONENTES: - ESP32 - LCD I2C 16x2 - Sensor DS18B20 - Sensor de pH -
Sensor de turbidez - LEDs - Buzzer - Relé - Pulsador

CODIGO FUENTE:
-codigo fuente arduino se encuentra en: https://github.com/atroutg/MonitoreoRioCesar/tree/main/src

FUNCIONALIDADES: 
- Lectura de sensores cada segundo 
- Evaluación de rangos normales
- Activación de alertas 
- Visualización en LCD (modo secuencial y scroll) 
- Simulación de envío de SMS

RANGOS: 
- pH: 6.5 - 9.0 
- Turbidez: < 50 NTU 
- Temperatura: ≤ 32 °C

CARACTERÍSTICAS: 
- Programación no bloqueante con millis() 
- Uso de interrupciones 
- Sistema modular

USO: 
1. Conectar los componentes 
2. Cargar el código en el ESP32 
3. Abrir monitor serial 
4. Observar resultados



LICENCIA: Proyecto académico - UNAD 2026




