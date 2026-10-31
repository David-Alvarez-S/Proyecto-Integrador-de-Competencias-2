import sys
import re
import serial
from pathlib import Path

from PySide6.QtCore import QObject, Signal, Slot, Property, QThread
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
                # readline() bloquea hasta recibir '\n' o alcanzar el timeout
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
    conveyor1Changed = Signal(bool)
    conveyor2Changed = Signal(bool)
    servo1StateChanged = Signal(str)
    ultra1Changed = Signal(bool)
    ultra2Changed = Signal(bool)
    palletColorChanged = Signal(str)
    pwmChanged = Signal(list)
    anglesChanged = Signal(list)
    serialLogChanged = Signal(str)

    def __init__(self):
        super().__init__()
        self._status = "[ DESCONECTADO ]"
        self._c1 = False
        self._c2 = False
        self._s1 = "[ QUIETO ]"
        self._u1 = False
        self._u2 = False
        self._color = "[ INDEFINIDO ]"
        self._pwm = [0, 0, 0, 0]
        self._angles = [0, 0, 0, 0]  # Base, Hombro, Codo, Garra

        self._log_lines = []
        self._serial_log = ""

        self.serial_port = None
        self.thread = None
        self.worker = None

        self.esp_log_regex = re.compile(r'^[IWEV]\s*\(\d+\)\s*([^:]+):\s*(.*)$')

    @Slot(str, str)
    def connect_serial(self, port, baudrate):
        if self.serial_port and self.serial_port.is_open:
            self.close_serial()

        try:
            baud = int(baudrate)
            self.serial_port = serial.Serial(port, baud, timeout=1)

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

    @Slot()
    def start_system(self):
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write(b"START\n")
                self._status = "SISTEMA ACTIVO"
                self.portStatusChanged.emit(self._status)
            except Exception as e:
                self.handle_serial_error(str(e))
        else:
            self._status = "ERROR: PUERTO NO CONECTADO"
            self.portStatusChanged.emit(self._status)

    @Slot()
    def stop_system(self):
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write(b"STOP\n")
                self._status = "SISTEMA DETENIDO"
                self.portStatusChanged.emit(self._status)
            except Exception as e:
                self.handle_serial_error(str(e))
        else:
            self._status = "ERROR: PUERTO NO CONECTADO"
            self.portStatusChanged.emit(self._status)

    @Slot(str)
    def append_log(self, line):
        self._log_lines.append(line)
        if len(self._log_lines) > 10:
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
                pwm_list = [int(x.strip()) for x in msg.split(',')]
                if len(pwm_list) == 4:
                    self._pwm = pwm_list
                    self.pwmChanged.emit(self._pwm)
            except ValueError:
                pass

        elif tag in ["ANGLES", "ANGULOS", "GRADOS"]:
            try:
                angles_list = [int(x.strip()) for x in msg.split(',')]
                if len(angles_list) == 4:
                    self._angles = angles_list
                    self.anglesChanged.emit(self._angles)
            except ValueError:
                pass

    def handle_serial_error(self, err_msg):
        self._status = f"ERROR SERIAL: {err_msg}"
        self.portStatusChanged.emit(self._status)
        self.close_serial()

    # --- PROPIEDADES GENERALES ---
    @Property(str, notify=portStatusChanged)
    def portStatus(self): return self._status

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

    # --- PROPIEDADES INDIVIDUALES DE LOS ÁNGULOS DEL BRAZO (Para vincular directo en QML) ---
    @Property(int, notify=anglesChanged)
    def anguloBase(self): return self._angles[0] if len(self._angles) > 0 else 0

    @Property(int, notify=anglesChanged)
    def anguloHombro(self): return self._angles[1] if len(self._angles) > 1 else 0

    @Property(int, notify=anglesChanged)
    def anguloCodo(self): return self._angles[2] if len(self._angles) > 2 else 0

    @Property(int, notify=anglesChanged)
    def anguloGarra(self): return self._angles[3] if len(self._angles) > 3 else 0


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