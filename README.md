# Intercambiador de Contenedores en Estación Final - Planta DLWL-800A

![Estado del Proyecto](https://img.shields.io/badge/Estado-En_Desarrollo-yellow)
![Microcontrolador](https://img.shields.io/badge/Control-ESP32-blue)
![ROS2](https://img.shields.io/badge/ROS2-C%2B%2B_%2F_RViz2-22314E)
![GUI](https://img.shields.io/badge/HMI-Qt_%2F_QML_%2F_PySide6-41CD52)
![Lenguajes](https://img.shields.io/badge/Lenguajes-C_%2F_C%2B%2B_%2F_Python_%2F_R-green)

Prototipo automatizado a escala (40 × 40 × 40 cm) para la clasificación y almacenaje continuo de pallets en la estación final de la planta industrial DLWL-800A. El sistema identifica pallets por color, gestiona un buffer de acumulación y reubica las cargas en contenedores fijos mediante un brazo robótico de 4 grados de libertad controlado en lazo cerrado, integrando simulación y análisis dinámico en **ROS 2**, mapeo de torque con **KDL y R (`ggplot2`)**, e interfaz HMI con gemelo digital en **Qt/QML**.

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
* **Drive:** https://drive.google.com/drive/folders/1wYajqpO4K8P1RmDh_BZdPhU3djc_-1ak?usp=sharing

---

## Arquitectura General y Solución Adoptada

La problemática principal radica en gestionar el almacenamiento final de pallets sin interrumpir el flujo proveniente de las estaciones previas. El sistema se divide en cuatro subsistemas integrados:

1. **Control Embebido en Tiempo Real (ESP32):**
   * **Transporte y Clasificación:** Dos cintas transportadoras impulsadas por motores paso a paso mediante registros de desplazamiento (`74HC595`), un sensor de color `TCS3200` para clasificar pallets (Rojo, Azul, Amarillo), dos sensores ultrasónicos `HC-SR04` para detección en pistón y buffer, y un actuador desviador controlado por PWM (`PCA9685`).
   * **Manipulador Robótico de 4 GDL:** Cinemática inversa analítica (`IK`), planificación de trayectorias cartesianas y articulares con **Perfil S** (control de velocidad, aceleración y jerk), y lazo cerrado PID + compensación de gravedad + feedforward de velocidad con encoders magnéticos absolutos **AS5600** (12 bits) sobre bus I²C y drivers PWM `PCA9685`.
   * **Persistencia NVS:** Almacenamiento en memoria flash no volátil (`NVS`) de ganancias PID, compensación gravitatoria, coordenadas de trabajo y tiempos de transporte.

2. **Ecosistema ROS 2 (Control, Simulación y Dinámica):**
   * **`nodo_control_brazo` (C++):** Servidor de acciones (`rclcpp_action`) bajo la interfaz `control_brazo/action/MoverBrazo` que valida alcanzabilidad cinemática, ejecuta interpolación con Perfil S a 100 Hz, publica feedback de progreso y transmite estados articulares a `/joint_states`.
   * **`nodo_mapeador_torque` (C++ + KDL):** Carga el modelo `robot.urdf` mediante `urdf` y `kdl_parser`, barre el plano radial-vertical $(R, Z)$ del espacio de trabajo resolviendo cinemática directa (`ChainFkSolverPos_recursive`) y dinámica inversa recursiva de Newton-Euler (`ChainIdSolver_RNE`) con carga útil en herramienta, animando el recorrido en RViz2 y exportando los esfuerzos articulares a `torques_plano_rz.csv`.
   * **Archivos Launch y Calibración:**
     * `action.launch.py`: Levanta `robot_state_publisher`, `nodo_control_brazo` y `rviz2`.
     * `display.launch.py`: Ejecuta el mapeador de torque junto con `robot_state_publisher` y `rviz2`.
     * `calibrar.launch.py`: GUI básica de calibración articular mediante `joint_state_publisher_gui` y `rviz2` para verificación visual de límites y ceros del URDF.

3. **Análisis de Esfuerzos y Mapas de Calor (R + `ggplot2`):**
   * El script `r_graph.R` procesa el archivo `torques_plano_rz.csv` generado por ROS 2 y calcula el porcentaje de exigencia estática respecto al torque *stall* de cada servomotor (Hombro, Codo, Muñeca) y la envolvente crítica global, exportando mapas de calor de alta resolución con isolíneas y umbrales de saturación (80 % y 100 %).

4. **HMI y Gemelo Digital en PC (Python + Qt / QML):**
   * Aplicación desarrollada en **PySide6 (Qt/QML)** (`main.py`) conectada por puerto serie UART al ESP32.
   * Permite visualizar un **gemelo digital** sincronizado en tiempo real con las lecturas de los encoders AS5600, monitorear PWM por eje, estado de cintas, sensores ultrasónicos y color detectado, ejecutar pruebas por color (`TEST ROJO/AZUL/AMARILLO`), calibrar PID/gravedad en caliente, ajustar coordenadas XYZ y guardar/restaurar parámetros en la memoria NVS del microcontrolador.

---

## Especificaciones Técnicas y Componentes

| Categoría | Componente / Herramienta | Descripción |
| :--- | :--- | :--- |
| **Control Principal** | ESP32 (ESP-IDF / FreeRTOS) | Ejecución determinista a 100 Hz del lazo de control articular, IK, Perfil S, telemetría y NVS. |
| **Sensado Angular** | 4 × Encoders Magnéticos AS5600 | Retroalimentación absoluta por eje (Base, Hombro, Codo, Muñeca) con filtrado adaptativo. |
| **Actuación Brazo** | Controlador PCA9685 + Servomotores | Generación PWM de alta resolución para servos de rotación continua/posicional. |
| **Cintas y Desvío** | 2 × Motores Paso a Paso (`74HC595`) + Servo Pistón | Transporte en cinta principal y cinta buffer con aceleración controlada. |
| **Sensado de Planta** | Sensor Color TCS3200 + 2 × HC-SR04 | Identificación RGB del pallet y detección de presencia en pistón y buffer. |
| **Middleware y Simulación** | ROS 2 (C++) + KDL + RViz2 | Action server `mover_brazo`, solver dinámico Newton-Euler y visualización 3D URDF. |
| **Análisis Gráfico** | R (`ggplot2`, `viridis`, `dplyr`) | Generación de mapas de calor de exigencia de torque en el plano $(R, Z)$. |
| **HMI y Gemelo Digital** | Python 3 + PySide6 (Qt / QML) | Interfaz de supervisión, control manual, gemelo digital y configuración NVS. |
| **Indicadores Físicos** | Pantalla TFT GC9306 + LEDs (`74HC595`) | Visualización local de estado e indicadores luminosos por color de pallet. |

---

## Ejecución de los Módulos de Software

### 1. ROS 2: Control por Action Server, Calibración y Mapeo de Torque

```bash
# Compilar paquetes de descripción y control
colcon build --packages-select descripcion_brazo control_brazo
source install/setup.bash

# Lanzar servidor de acciones 'mover_brazo' + RViz2
ros2 launch control_brazo action.launch.py

# Enviar una meta cartesiana (X, Y, Z en metros) desde otra terminal
ros2 action send_goal /mover_brazo control_brazo/action/MoverBrazo "{x: 0.22, y: 0.05, z: 0.12}" --feedback

# Lanzar GUI básica de calibración articular en RViz2
ros2 launch control_brazo calibrar.launch.py

# Ejecutar mapeo animado de torque KDL y exportar CSV para R
ros2 launch control_brazo display.launch.py
```

### 2. Generación de Mapas de Calor de Torque (R)

Una vez generado `graficas/torques_plano_rz.csv` mediante `display.launch.py`:

```bash
Rscript r_graph.R
```

Esto genera en `graficas/` las 4 figuras de exigencia estática (`figura_1_torque_hombro.png` a `figura_4_torque_global_critico.png`).

### 3. Interfaz Gráfica HMI y Gemelo Digital (Qt / QML)

```bash
pip install PySide6 pyserial
python3 main.py
```

Desde la interfaz se selecciona el puerto serie del ESP32 (`COMx` / `/dev/ttyUSB0`) para sincronizar automáticamente la configuración NVS (`GET_CFG`), visualizar el gemelo digital en tiempo real y operar en modo **Automático** o **Manual**.

---

## Modos de Operación

* **Modo Automático (`START`):** Detección de presencia por ultrasónico, lectura de color con TCS3200, desvío hacia el buffer mediante el pistón, recolección con trayectoria cartesiana Perfil S y depósito en el contenedor correspondiente (Rojo, Azul o Amarillo).
* **Modo Manual / Calibración:** Control individual por eje (`ANG`), pruebas de ciclo por color (`TEST ROJO/AZUL/AMARILLO`), ajuste en tiempo real de ganancias (`PID`, `GRAV`), modificación de puntos de trabajo (`PICK`, `CONT`, `ZCOLOR`) y guardado permanente en flash (`SAVE_NVS`).
