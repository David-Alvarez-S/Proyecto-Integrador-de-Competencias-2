# Intercambiador de Contenedores en Estación Final - Planta DLWL-800A

![Estado del Proyecto](https://img.shields.io/badge/Estado-En_Desarrollo-yellow)
![Microcontrolador](https://img.shields.io/badge/Control-ESP32-blue)
![Lenguaje](https://img.shields.io/badge/Lenguaje-C%20%2F%20C%2B%2B%20%2F%20Python-green)

Prototipo automatizado a escala (40 × 40 × 40 cm) para la clasificación y almacenaje continuo de pallets en la estación final de la planta industrial **DLWL-800A**. El sistema permite identificar pallets por color y reubicarlos en contenedores fijos mediante un brazo robótico, incorporando un buffer para evitar detener el flujo de la línea de producción.

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

La problemática principal radica en gestionar el almacenamiento final de pallets sin interrumpir el proceso de las estaciones previas. Para lograrlo, se implementó una **solución basada en clasificación por color y manipulación mediante un brazo robótico**:

1. **Ingreso y Lectura:** Los pallets (dimensiones a escala de 5 × 4.2 × 1 cm) ingresan a través de una cinta transportadora principal impulsada por motores DC. Un sensor de color (TCS3200) identifica la categoría del pallet.
2. **Buffer de Acumulación:** Se incluye una cinta secundaria para acumular temporalmente los pallets entrantes mientras el manipulador realiza las operaciones de descarga o reordenamiento.
3. **Clasificación por Brazo Robótico:** Se seleccionó un brazo robótico articulado accionando servomotores MG90S, equipado con una herramienta tipo horquilla (estilo montacargas) que recoge el pallet desde el buffer y lo deposita en su respectivo contenedor.
4. **Visualización y Modos de Operación:** Cuenta con un modo de operación automático y manual, supervisados mediante una Interfaz Gráfica de Usuario (GUI en Python) y señales luminosas/acústicas en el hardware.
