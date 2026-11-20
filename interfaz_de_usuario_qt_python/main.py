import sys
import re
import serial
import serial.tools.list_ports
from pathlib import Path

from PySide6.QtCore import QObject, Signal, Slot, Property, QThread, QTimer
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine


class SerialReaderWorker(QObject):
    line_received = Signal(str)
    error_occurred = Signal(str)

    def __init__(self, serial_instance):
        super().__init__()
        self.serial = serial_instance
        self.running = True

    def run(self):
        while self.running and self.serial and self.serial.is_open:
            try:
                line = self.serial.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    self.line_received.emit(line)
            except Exception as e:
                self.error_occurred.emit(str(e))
                break

    def stop(self):
        self.running = False


class Backend(QObject):
    portStatusChanged = Signal(str)
    availablePortsChanged = Signal(list)
    conveyor1Changed = Signal(bool)
    conveyor2Changed = Signal(bool)
    servo1StateChanged = Signal(str)
    ultra1Changed = Signal(bool)
    ultra2Changed = Signal(bool)
    palletColorChanged = Signal(str)
    pwmChanged = Signal(list)
    anglesChanged = Signal(list)
    serialLogChanged = Signal(str)
    configLoaded = Signal()

    def __init__(self):
        super().__init__()
        self._status = "[ DESCONECTADO ]"
        self._ports = []
        self._c1 = False
        self._c2 = False
        self._s1 = "[ QUIETO ]"
        self._u1 = False
        self._u2 = False
        self._color = "[ INDEFINIDO ]"
        self._pwm = [0, 0, 0, 0]
        self._angles = [0, 0, 0, 0]
        self._angles_float = [0.0, 0.0, 0.0, 0.0]

        # Configuración cargada desde NVS/ESP32
        self._pid_params = [
            [2.10, 0.70, 0.30, 0.0],   # Base
            [1.80, 0.60, 0.45, 18.0],  # Hombro
            [2.20, 0.60, 0.38, 12.0],  # Codo
            [1.90, 0.50, 0.30, 4.0],   # Muñeca
        ]
        self._pos_params = {
            "pickX": 0.260, "pickY": 0.000, "pickZ": 0.060,
            "liftZ": 0.050,
            "contX": 0.000, "contY": 0.260,
            "zRojo": 0.070, "zAzul": 0.140, "zAmarillo": 0.210,
            "avance": 0.025, "bajada": 0.020,
            "pistonRetardo": 4300, "pistonExt": 1000, "pistonRet": 1850,
            "velCinta": 450.0
        }

        self._log_lines = []
        self._serial_log = ""

        self.serial_port = None
        self.thread = None
        self.worker = None

        self.esp_log_regex = re.compile(r'^[IWEV]\s*\(\d+\)\s*([^:]+):\s*(.*)$')
        self.refresh_ports()

    @Slot()
    def refresh_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self._ports = ports if ports else ["COM3", "/dev/ttyUSB0"]
        self.availablePortsChanged.emit(self._ports)

    @Slot(str)
    def send_raw_command(self, cmd):
        clean_cmd = cmd.strip()
        if not clean_cmd:
            return
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write((clean_cmd + "\n").encode('utf-8'))
                self.append_log(f">> {clean_cmd}")
            except Exception as e:
                self.handle_serial_error(str(e))
        else:
            self._status = "ERROR: PUERTO NO CONECTADO"
            self.portStatusChanged.emit(self._status)

    @Slot(str, str)
    def connect_serial(self, port, baudrate):
        if self.serial_port and self.serial_port.is_open:
            self.close_serial()

        try:
            baud = int(baudrate)
            self.serial_port = serial.Serial(port.strip(), baud, timeout=1)

            self.thread = QThread()
            self.worker = SerialReaderWorker(self.serial_port)
            self.worker.moveToThread(self.thread)

            self.thread.started.connect(self.worker.run)
            self.worker.line_received.connect(self.parse_esp_log)
            self.worker.line_received.connect(self.append_log)
            self.worker.error_occurred.connect(self.handle_serial_error)

            self.thread.start()

            self._status = f"CONECTADO: {port} @ {baud}"
            self.portStatusChanged.emit(self._status)

            # Solicitar configuración actual almacenada en NVS del ESP32
            QTimer.singleShot(350, lambda: self.send_raw_command("GET_CFG"))

        except Exception as e:
            self._status = f"ERROR CONEXIÓN: {str(e)}"
            self.portStatusChanged.emit(self._status)

    @Slot()
    def close_serial(self):
        if self.worker:
            self.worker.stop()
        if self.thread:
            self.thread.quit()
            self.thread.wait()
            self.thread = None
            self.worker = None
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()

        self._pwm = [0, 0, 0, 0]
        self.pwmChanged.emit(self._pwm)

    @Slot()
    def start_system(self):
        self.send_raw_command("START")
        if self.serial_port and self.serial_port.is_open:
            self._status = "SISTEMA ACTIVO"
            self.portStatusChanged.emit(self._status)

    @Slot()
    def stop_system(self):
        self.send_raw_command("STOP")
        if self.serial_port and self.serial_port.is_open:
            self._status = "SISTEMA DETENIDO"
            self.portStatusChanged.emit(self._status)
            self._pwm = [0, 0, 0, 0]
            self.pwmChanged.emit(self._pwm)

    # --- SLOTS DE CONTROL MANUAL Y CALIBRACIÓN NVS ---
    @Slot()
    def home_arm(self):
        self.send_raw_command("HOME")

    @Slot()
    def hold_arm(self):
        self.send_raw_command("HOLD")

    @Slot(int, float)
    def move_axis_deg(self, axis_idx, deg):
        self.send_raw_command(f"ANG {axis_idx + 1} {deg:.1f}")

    @Slot(str)
    def run_color_test(self, color_name):
        self.send_raw_command(f"TEST {color_name}")

    @Slot(int, float, float, float, float)
    def apply_axis_pid(self, axis_idx, kp, ki, kd, grav):
        self._pid_params[axis_idx] = [kp, ki, kd, grav]
        self.send_raw_command(f"PID {axis_idx + 1} {kp:.3f} {ki:.3f} {kd:.3f}")
        self.send_raw_command(f"GRAV {axis_idx + 1} {grav:.2f}")

    @Slot(float, float, float, float, float, float, float, float, float, float, float)
    def apply_positions(self, px, py, pz, lift, cx, cy, zr, za, zam, avance, bajada):
        self._pos_params.update({
            "pickX": px, "pickY": py, "pickZ": pz, "liftZ": lift,
            "contX": cx, "contY": cy,
            "zRojo": zr, "zAzul": za, "zAmarillo": zam,
            "avance": avance, "bajada": bajada
        })
        self.send_raw_command(f"PICK {px:.3f} {py:.3f} {pz:.3f}")
        self.send_raw_command(f"LIFT {lift:.3f}")
        self.send_raw_command(f"CONT {cx:.3f} {cy:.3f}")
        self.send_raw_command(f"ZCOLOR {zr:.3f} {za:.3f} {zam:.3f}")
        self.send_raw_command(f"ASENTAR {avance:.3f} {bajada:.3f}")

    @Slot(int, int, int, float)
    def apply_transport_cfg(self, retardo_ms, ext_ms, ret_ms, vel_cinta):
        self._pos_params.update({
            "pistonRetardo": retardo_ms,
            "pistonExt": ext_ms,
            "pistonRet": ret_ms,
            "velCinta": vel_cinta
        })
        self.send_raw_command(f"PISTON_CFG {retardo_ms} {ext_ms} {ret_ms} {vel_cinta:.1f}")

    @Slot()
    def save_to_nvs(self):
        self.send_raw_command("SAVE_NVS")

    @Slot()
    def reset_nvs(self):
        self.send_raw_command("RESET_NVS")

    @Slot()
    def request_config(self):
        self.send_raw_command("GET_CFG")

    @Slot(str)
    def append_log(self, line):
        is_telemetry = ("ANGULOS:" in line or "PWM:" in line)
        if is_telemetry and self._log_lines and ("ANGULOS:" in self._log_lines[-1] or "PWM:" in self._log_lines[-1]):
            self._log_lines[-1] = line
        else:
            self._log_lines.append(line)
            if len(self._log_lines) > 14:
                self._log_lines.pop(0)

        self._serial_log = "\n".join(self._log_lines)
        self.serialLogChanged.emit(self._serial_log)

    @Slot(str)
    def parse_esp_log(self, raw_line):
        match = self.esp_log_regex.match(raw_line)
        if not match:
            return

        tag = match.group(1).strip().upper()
        msg = match.group(2).strip()

        if tag in ["CONVEYOR1", "CINTA1"]:
            val = msg in ["1", "true", "TRUE", "ON", "SI"]
            if val != self._c1:
                self._c1 = val
                self.conveyor1Changed.emit(self._c1)

        elif tag in ["CONVEYOR2", "CINTA2"]:
            val = msg in ["1", "true", "TRUE", "ON", "SI"]
            if val != self._c2:
                self._c2 = val
                self.conveyor2Changed.emit(self._c2)

        elif tag in ["SERVO1", "PISTON"]:
            formatted_s1 = f"[ {msg.upper()} ]"
            if formatted_s1 != self._s1:
                self._s1 = formatted_s1
                self.servo1StateChanged.emit(self._s1)

        elif tag in ["ULTRA1", "ULTRASONICO1"]:
            val = msg in ["1", "true", "TRUE", "DETECTADO", "SI"]
            if val != self._u1:
                self._u1 = val
                self.ultra1Changed.emit(self._u1)

        elif tag in ["ULTRA2", "ULTRASONICO2"]:
            val = msg in ["1", "true", "TRUE", "DETECTADO", "SI"]
            if val != self._u2:
                self._u2 = val
                self.ultra2Changed.emit(self._u2)

        elif tag in ["COLOR", "PALLET_COLOR"]:
            formatted_color = f"[ {msg.upper()} ]"
            if formatted_color != self._color:
                self._color = formatted_color
                self.palletColorChanged.emit(self._color)

        elif tag == "PWM":
            try:
                pwm_list = [max(-100, min(100, int(round(float(x.strip()))))) for x in msg.split(',')]
                if len(pwm_list) == 4 and pwm_list != self._pwm:
                    self._pwm = pwm_list
                    self.pwmChanged.emit(self._pwm)
            except ValueError:
                pass

        elif tag in ["ANGLES", "ANGULOS", "GRADOS"]:
            try:
                raw_vals = [max(0.0, min(360.0, float(x.strip()))) for x in msg.split(',')]
                if len(raw_vals) == 4:
                    angles_int = [int(round(v)) % 360 for v in raw_vals]
                    if angles_int != self._angles or raw_vals != self._angles_float:
                        self._angles = angles_int
                        self._angles_float = raw_vals
                        self.anglesChanged.emit(self._angles)
            except ValueError:
                pass

        elif tag == "CFG_PID":
            try:
                parts = [float(x.strip()) for x in msg.split(',')]
                if len(parts) == 5:
                    idx = int(parts[0]) - 1
                    if 0 <= idx < 4:
                        self._pid_params[idx] = parts[1:5]
                        self.configLoaded.emit()
            except ValueError:
                pass

        elif tag == "CFG_POS":
            try:
                p = [float(x.strip()) for x in msg.split(',')]
                if len(p) == 10:
                    self._pos_params.update({
                        "pickX": p[0], "pickY": p[1], "pickZ": p[2],
                        "liftZ": p[3], "contX": p[4], "contY": p[5],
                        "zRojo": p[6], "zAzul": p[7], "zAmarillo": p[8],
                        "avance": p[9]
                    })
                    self.configLoaded.emit()
            except ValueError:
                pass

        elif tag == "CFG_CINTA":
            try:
                p = [float(x.strip()) for x in msg.split(',')]
                if len(p) == 5:
                    self._pos_params.update({
                        "pistonRetardo": int(p[0]),
                        "pistonExt": int(p[1]),
                        "pistonRet": int(p[2]),
                        "velCinta": p[3],
                        "bajada": p[4]
                    })
                    self.configLoaded.emit()
            except ValueError:
                pass

        elif tag == "NVS":
            if "SAVED_OK" in msg:
                self._status = "NVS: PARÁMETROS GUARDADOS EN FLASH"
            elif "RESET_OK" in msg:
                self._status = "NVS: MEMORIA RESTAURADA A FÁBRICA"
            self.portStatusChanged.emit(self._status)

    def handle_serial_error(self, err_msg):
        self._status = f"ERROR SERIAL: {err_msg}"
        self.portStatusChanged.emit(self._status)
        self.close_serial()

    # --- PROPIEDADES QML ---
    @Property(str, notify=portStatusChanged)
    def portStatus(self): return self._status

    @Property(list, notify=availablePortsChanged)
    def availablePorts(self): return self._ports

    @Property(bool, notify=conveyor1Changed)
    def conveyor1(self): return self._c1

    @Property(bool, notify=conveyor2Changed)
    def conveyor2(self): return self._c2

    @Property(str, notify=servo1StateChanged)
    def servo1State(self): return self._s1

    @Property(bool, notify=ultra1Changed)
    def ultra1(self): return self._u1

    @Property(bool, notify=ultra2Changed)
    def ultra2(self): return self._u2

    @Property(str, notify=palletColorChanged)
    def palletColor(self): return self._color

    @Property(list, notify=pwmChanged)
    def pwmData(self): return self._pwm

    @Property(list, notify=anglesChanged)
    def anglesData(self): return self._angles

    @Property(str, notify=serialLogChanged)
    def serialLog(self): return self._serial_log

    @Property(list, notify=configLoaded)
    def pidParams(self): return self._pid_params

    @Property('QVariantMap', notify=configLoaded)
    def posParams(self): return self._pos_params

    @Property(int, notify=anglesChanged)
    def anguloBase(self): return self._angles[0]

    @Property(int, notify=anglesChanged)
    def anguloHombro(self): return self._angles[1]

    @Property(int, notify=anglesChanged)
    def anguloCodo(self): return self._angles[2]

    @Property(int, notify=anglesChanged)
    def anguloGarra(self): return self._angles[3]


if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    engine = QQmlApplicationEngine()

    backend = Backend()
    engine.rootContext().setContextProperty("backend", backend)

    engine.addImportPath(Path(__file__).parent)
    engine.loadFromModule("interfaz_de_usuario_qt_python", "Main")

    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec())