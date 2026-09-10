# Intercambiador de Contenedores en Estación Final - Planta DLWL-800A

![Estado del Proyecto](https://img.shields.io/badge/Estado-En_Desarrollo-yellow)
![Microcontrolador](https://img.shields.io/badge/Control-ESP32-blue)
![Lenguaje](https://img.shields.io/badge/Lenguaje-C%20%2F%20C%2B%2B%20%2F%20Python-green)

Prototipo automatizado a escala (40 × 40 × 40 cm) para la clasificación y almacenaje continuo de pallets en la estación final de la planta industrial DLWL-800A. El sistema permite identificar pallets por color y reubicarlos en contenedores fijos mediante un brazo robótico, incorporando un buffer para evitar detener el flujo de la línea de producción.

---

## Integrantes

* **Desarrolladores:** 
  * Damián Bonilla 
  * Matías Hernández 
  * David Alvarez 
* **Institución:** Instituto Tecnológico Regional (ITR) Suroeste - Sede Fray Bentos
* **Carrera:** Ingeniería en Mecatrónica
* **Unidad Curricular:** Proyecto Integrador de Competencias II (PIC II)
* **Docentes:** Leonardo Eguia, Marcelo Diaz, Andres Hippa, Rodrigo Sosa

---

## Descripción del Proyecto y Solución Adoptada

La problemática principal radica en gestionar el almacenamiento final de pallets sin interrumpir el proceso de las estaciones previas. Para lograrlo, se implementó una solución basada en clasificación por color y manipulación mediante un brazo robótico:

1. **Ingreso y Lectura:** Los pallets (dimensiones a escala de 5 × 4.2 × 1 cm) ingresan a través de una cinta transportadora principal impulsada por motores DC. Un sensor de color (TCS3200) identifica la categoría del pallet.
2. **Buffer de Acumulación:** Se incluye una cinta secundaria para acumular temporalmente los pallets entrantes mientras el manipulador realiza las operaciones de descarga o reordenamiento.
3. **Clasificación por Brazo Robótico:** Se seleccionó un brazo robótico articulado accionando servomotores MG90S, equipado con una herramienta tipo horquilla (estilo montacargas) que recoge el pallet desde el buffer y lo deposita en su respectivo contenedor.
4. **Visualización y Modos de Operación:** Cuenta con un modo de operación automático y manual, supervisados mediante una Interfaz Gráfica de Usuario (usando Python) y señales luminosas/acústicas en el hardware.

---

## Objetivos del Proyecto

* **Objetivo General:** Desarrollar un prototipo automatizado que clasifique y almacene pallets en contenedores según su color, manteniendo la continuidad del flujo proveniente de estaciones previas.
* **Objetivos Específicos:**
  * Diseñar y fabricar la estructura física mediante impresión 3D.
  * Desarrollar la lógica de control para coordinar las cintas, el buffer y la cinemática del brazo robótico.
  * Diseñar una interfaz gráfica (en python) para la supervisión y control manual del proceso.
  * Validar la clasificación y reordenamiento continuo sin detenciones del flujo.

---

## Especificaciones Técnicas y Componentes

| Categoria | Componente / Herramienta | Descripción |
| :--- | :--- | :--- |
| **Control Principal** | ESP32 | Microcontrolador encargado de la lógica, sensado, PWM de servos y comunicación serie. |
| **Actuador Principal** | Brazo Robótico con 6 Servomotores MG90S | Sistema de manipulación estilo montacargas para la toma y descarga de pallets. |
| **Actuadores Secundarios** | Motores DC | Accionamiento de cinta transportadora de llegada y cinta de buffer. |
| **Sensado** | Sensor de Color TCS3200 | Lectura e identificación de la categoría del pallet por color. |
| **Estructura y Chasis** | Impresión 3D (PLA) | Estructura a escala de la estación con dimensiones máximas de 40 × 40 × 40 cm. |
| **Indicadores Visuales** | LEDs RGB / LEDs de Estado | **Verde:** Operación normal / **Azul:** Contenedor lleno / **Amarillo:** Intercambio en curso / **Rojo:** Falla. |
| **Indicadores Acústicos**| Buzzer Activo | Avisos sonoros (1 tono: próximo a lleno / 1 tono: intercambio exitoso / 2 tonos: falla). |
| **Software y GUI** | Python y C++| Firmware de la ESP32 y dashboard de visualización para PC. |

---

## Modos de Operación

* **Modo Automático:** Detección de color por sensor, desvío automático hacia el buffer, recolección con el brazo robótico y depósito en el contenedor correspondiente de manera autónoma.
* **Modo Manual:** Control directo de los actuadores, avance de cintas e intercambio de contenedores desde la Interfaz Gráfica de Usuario en la PC.

---
