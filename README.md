# Sistema Embebido Inteligente para Monitoreo del Río Cesar

## Información General

- **Universidad:** Universidad Nacional Abierta y a Distancia (UNAD)  
- **Programa:** Ingeniería Electrónica  
- **Asignatura:** Microprocesadores y Microcontroladores  
- **Grupo:** 309696_1  
- **Fase:** Fase 3 - Diseño y Desarrollo de la Solución  

---

## Descripción del Proyecto

Este proyecto desarrolla un sistema embebido basado en **ESP32** para el monitoreo ambiental del **Río Cesar**.  

El sistema permite medir variables críticas como:

- pH del agua  
- Turbidez  
- Temperatura  

Además, integra mecanismos de alerta para detectar posibles condiciones de contaminación, mediante:

- Alertas visuales (LEDs)  
- Alertas sonoras (buzzer)  
- Notificaciones digitales (simulación de SMS)  

Los datos son visualizados en una pantalla LCD, facilitando la supervisión en tiempo real.

---

## Integrantes

- Armando Guillermo Trout García  
- Jordan Javier Rada Cardona  
- Lainiker Eduardo Yanez Gutierrez  
- Sebastián Machado Peralta  

---

## Tutor

- Ing. Julio Cesar Vidal  

---

## Componentes del Sistema

- ESP32  
- LCD I2C 16x2  
- Sensor de temperatura DS18B20  
- Sensor de pH  
- Sensor de turbidez  
- LEDs indicadores  
- Buzzer  
- Relé  
- Pulsador  

---

## Funcionalidades

- Lectura periódica de sensores (cada 1 segundo)  
- Evaluación automática de rangos normales  
- Activación de alertas ante condiciones críticas  
- Visualización en LCD:
  - Modo secuencial  
  - Modo scroll  
- Simulación de envío de SMS  
- Programación no bloqueante usando `millis()`  
- Uso de interrupciones  
- Arquitectura modular  

---

## Rangos de Referencia

| Variable     | Rango Normal        |
|-------------|--------------------|
| pH          | 6.5 – 9.0          |
| Turbidez    | < 50 NTU           |
| Temperatura | ≤ 32 °C            |

---

## Código Fuente

El código del proyecto está disponible en el siguiente repositorio:

🔗 https://github.com/atroutg/MonitoreoRioCesar/tree/main/src  

---

## Instrucciones de Uso

1. Conectar correctamente todos los componentes al ESP32  
2. Cargar el código en la tarjeta mediante el IDE de Arduino  
3. Abrir el monitor serial  
4. Verificar la lectura de sensores y estado del sistema  

---

## Características Técnicas

- Sistema en tiempo real no bloqueante  
- Bajo consumo de recursos  
- Escalable y modular  
- Adaptable a plataformas IoT  

---

## Licencia

Proyecto académico desarrollado en el marco de la  
**Universidad Nacional Abierta y a Distancia (UNAD) – 2026**
