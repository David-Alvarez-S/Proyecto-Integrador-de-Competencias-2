import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers // Permite rotar/orbitar la cámara con el ratón

Window {
    width: 1280
    height: 720
    visible: true
    title: "PIC 2 - Sistema de Monitoreo"
    color: "#000000" // Negro puro

    // Paleta HUD Futurista
    property color bgPanel: "#000000"
    property color borderColor: "#222222"
    property color textColor: "#f5f5f5"
    property color textDim: "#9E9E9E"
    property color accent: "#e0e0e0" // Cian futurista
    property color alertColor: "#ff3333"

    // Agrega esto en su lugar
    property string monoFont: "Courier New"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        // ================= CABECERA =================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            color: "transparent"
            border.color: borderColor

            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 20
                text: "[ Sistema de Monitoreo de Cadena Cinematica ]"
                color: accent
                font.family: monoFont.name
                font.pixelSize: 12
                font.bold: true
                font.letterSpacing: 2
            }

            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 20
                text: "PROYECTO DE: DAVID ALVAREZ, DAMIAN BONILLA, MATIAS HERNANDEZ"
                color: textDim
                font.family: monoFont.name
                font.pixelSize: 7
                font.letterSpacing: 1
                font.bold: true
            }
        }

        // ================= CUERPO =================
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 15

            // --- PANEL IZQUIERDO (Control) ---
            Rectangle {
                Layout.preferredWidth: 320
                Layout.fillHeight: true
                color: bgPanel
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 7

                    Text { text: " Configuracion Serial"; color: accent; font.family: monoFont.name; font.pixelSize: 10
                        font.bold: true; font.letterSpacing: 2
                    }

                    RowLayout {
                        TextField {
                            id: portInput
                            placeholderText: "COM / ttyUSB"
                            Layout.fillWidth: true
                            color: textColor
                            font.family: monoFont.name
                            font.pixelSize: 9
                            font.letterSpacing: 2
                            background: Rectangle { color: "transparent"; border.color: borderColor }
                        }
                        TextField {
                            id: baudInput
                            text: "115200"
                            Layout.preferredWidth: 80
                            color: textColor
                            font.family: monoFont.name
                            font.pixelSize: 9
                            font.letterSpacing: 2
                            background: Rectangle { color: "transparent"; border.color: borderColor }
                        }
                    }

                    Button {
                        text: "CONECTAR"
                        Layout.fillWidth: true
                        font.family: monoFont.name
                        contentItem: Text { text: parent.text; color: parent.pressed ? "#000" : accent; horizontalAlignment: Text.AlignHCenter; font.family: monoFont.name; font.pixelSize: 12; font.letterSpacing: 2}
                        background: Rectangle { color: parent.pressed ? accent : "transparent"; border.color: accent }
                        onClicked: backend.connect_serial(portInput.text, baudInput.text)
                        font.letterSpacing: 3
                    }

                    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: borderColor; Layout.topMargin: 10; Layout.bottomMargin: 10 }

                    Text { text: " EJECUCION DEL SISTEMA"; color: accent; font.family: monoFont.name; font.pixelSize: 10
                        font.bold: true; font.letterSpacing: 2
                    }

                    RowLayout {
                        Button {
                            text: "INICIAR"
                            Layout.fillWidth: true
                            contentItem: Text { text: parent.text; color: parent.pressed ? "#000" : accent; horizontalAlignment: Text.AlignHCenter; font.family: monoFont.name; font.pixelSize: 12; font.letterSpacing: 2 }
                            background: Rectangle { color: parent.pressed ? accent : "transparent"; border.color: borderColor }
                            onClicked: backend.start_system()
                            font.letterSpacing: 3
                        }
                        Button {
                            text: "PARAR"
                            Layout.fillWidth: true
                            contentItem: Text { text: parent.text; color: parent.pressed ? "#000" : alertColor; horizontalAlignment: Text.AlignHCenter; font.family: monoFont.name; font.pixelSize: 12; font.letterSpacing: 2 }
                            background: Rectangle { color: parent.pressed ? alertColor : "transparent"; border.color: borderColor }
                            onClicked: backend.stop_system()
                            font.letterSpacing: 3
                        }
                    }

                    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: borderColor; Layout.topMargin: 10; Layout.bottomMargin: 10 }

                    Text { text: " DATOS DE SENSORES "; color: accent; font.family: monoFont.name; font.pixelSize: 10
                        font.bold: true; font.letterSpacing: 2
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: 10
                        rowSpacing: 12
                        Layout.fillWidth: true

                        Text { text: "CINTA TRANSPORTADORA 1:"; color: textDim; font.family: monoFont.name; font.pixelSize: 9 }
                        Text { text: backend.conveyor1 ? "[ ACTIVO ]" : "[ PARADO ]"; color: textColor; font.family: monoFont.name; font.pixelSize: 9 ; font.letterSpacing: 2}

                        Text { text: "CINTA TRANSPORTADORA 2:"; color: textDim; font.family: monoFont.name; font.pixelSize: 9 }
                        Text { text: backend.conveyor2 ? "[ ACTIVO ]" : "[ PARADO ]"; color: textColor; font.family: monoFont.name; font.pixelSize: 9; font.letterSpacing: 2 }

                        Text { text: "ESTADO PISTON:"; color: textDim; font.family: monoFont.name; font.pixelSize: 9 }
                        Text { text: backend.servo1State.toUpperCase(); color: accent; font.family: monoFont.name; font.pixelSize: 9 ; font.letterSpacing: 2}

                        Text { text: "SENSOR ULTRASONICO 1:"; color: textDim; font.family: monoFont.name; font.pixelSize: 9 }
                        Text { text: backend.ultra1 ? "[ PALLET DETECTADO ]" : "[ VACIO ]"; color: textColor; font.family: monoFont.name; font.pixelSize: 9 ; font.letterSpacing: 2}

                        Text { text: "SENSOR ULTRASONICO 2:"; color: textDim; font.family: monoFont.name; font.pixelSize: 9 }
                        Text { text: backend.ultra2 ? "[ PALLET DETECTADO ]" : "[ VACIO ]"; color: textColor; font.family: monoFont.name; font.pixelSize: 9 ; font.letterSpacing: 2}

                        Text { text: "COLOR PALET:"; color: textDim; font.family: monoFont.name; font.pixelSize: 9 }
                        Text { text: backend.palletColor.toUpperCase(); color: textColor; font.family: monoFont.name; font.pixelSize: 9 ; font.letterSpacing: 2}
                    }

                    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: borderColor; Layout.topMargin: 10; Layout.bottomMargin: 5 }
                    Text {
                        text: " TERMINAL SERIAL"; color: accent; font.family: monoFont.name; font.pixelSize: 10
                        font.bold: true; font.letterSpacing: 2
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#050505"
                        border.color: borderColor
                        clip: true
                        ScrollView {
                            anchors.fill: parent
                            anchors.margins: 8
                            TextArea {
                                text: backend.serialLog
                                color: "#00ff00" // Verde terminal para contrastar con el cian (o usa 'accent')
                                font.family: monoFont.name
                                font.pixelSize: 8
                                readOnly: true
                                wrapMode: Text.WrapAnywhere
                                background: null
                                // Auto-scroll hacia abajo
                                onTextChanged: {
                                    cursorPosition = text.length
                                }
                            }
                        }
                    }
                }
            }

            // --- PANEL CENTRAL ---
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                border.color: "#000000"
                border.width: 1
                clip: true

                // Decoraciones esquinas (HUD)
                Rectangle { width: 15; height: 2; color: accent; anchors.top: parent.top; anchors.left: parent.left }
                Rectangle { width: 2; height: 15; color: accent; anchors.top: parent.top; anchors.left: parent.left }
                Rectangle { width: 15; height: 2; color: accent; anchors.bottom: parent.bottom; anchors.right: parent.right }
                Rectangle { width: 2; height: 15; color: accent; anchors.bottom: parent.bottom; anchors.right: parent.right }

                // DE ACA

                View3D {
                    id: view3D
                    anchors.fill: parent

                    environment: SceneEnvironment {
                        clearColor: "#000000"
                        backgroundMode: SceneEnvironment.Color // Mantiene el fondo negro
                        antialiasingMode: SceneEnvironment.MSAA

                        // Otorga reflejos de estudio al metal
                        lightProbe: Texture {
                            source: "images/studio_hdr.hdr" // Un archivo HDR o JPG con luces/gradientes
                        }
                        probeExposure: 1.5 // Intensidad del reflejo
                    }
                    PerspectiveCamera {
                        id: camera
                        position: Qt.vector3d(0, 20, 50)
                        eulerRotation.x: -3 // Prácticamente horizontal

                    }
                    // Luz de contorno (Rim Light) ubicada detrás del brazo
                    DirectionalLight {
                        eulerRotation.x: 30   // Angulada hacia abajo
                        eulerRotation.y: 180  // Apuntando desde ATRÁS hacia la cámara
                        brightness: 3.0       // Brillo alto para crear el borde metálico
                        color: "#ffffff"
                        castsShadow: false
                    }

                    DirectionalLight {
                        eulerRotation.x: -30
                        eulerRotation.y: -45
                        brightness: 1.2
                        castsShadow: false
                    }

                    DirectionalLight {
                        eulerRotation.x: 30
                        eulerRotation.y: 135
                        brightness: 0.8
                        castsShadow: false
                    }

                    DirectionalLight {
                        eulerRotation.x: -90
                        brightness: 0.5
                        castsShadow: false
                    }
                    // Luz de contorno (Rim Light) ubicada detrás del brazo
                    DirectionalLight {
                        eulerRotation.x: 30   // Angulada hacia abajo
                        eulerRotation.y: 180  // Apuntando desde ATRÁS hacia la cámara
                        brightness: 3.0       // Brillo alto para crear el borde metálico
                        color: "#ffffff"
                        castsShadow: false
                    }

                    OrbitCameraController {
                        origin: brazo
                        camera: camera
                    }

                    // Instancia del Gemelo Digital
                    Brazo_Robotico {
                        id: brazo
                        scale: Qt.vector3d(110, 110, 110)
                        eulerRotation.x: -90


                        anguloBase: backend.anguloBase
                        anguloHombro: backend.anguloHombro
                        anguloCodo: backend.anguloCodo
                        anguloGarra: backend.anguloGarra

                    }
                }
            }

            // --- PANEL DERECHO ---
            Rectangle {
                Layout.preferredWidth: 320
                Layout.fillHeight: true
                color: bgPanel
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text { text: "TELEMETRIA DEL BRAZO ROBOTICO"; color: accent; font.family: monoFont.name; font.pixelSize: 10
                        font.letterSpacing: 2; font.bold: true
                    }

                    // Gráficas de PWM (-100 a 100)
                    Text { text: "SEÑAL PWM [-100 a 100]"; color: textDim; font.family: monoFont; font.pixelSize: 9; font.letterSpacing: 2 }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 180
                        spacing: 10
                        Repeater {
                            model: 4
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 15; height: parent.height
                                    color: "transparent"
                                    border.color: borderColor
                                    // Línea indicadora del 0 (Centro)
                                    Rectangle {
                                        width: parent.width
                                        height: 1
                                        color: textDim
                                        anchors.centerIn: parent
                                    }
                                    Rectangle { // Relleno de barra HUD bidireccional
                                        width: parent.width - 2
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        // La altura máxima posible ahora es la mitad del contenedor
                                        height: (Math.abs(backend.pwmData[index]) / 100) * ((parent.height - 2) / 2)
                                        // Si es positivo, se dibuja del centro hacia arriba. Si es negativo, del centro hacia abajo.
                                        y: backend.pwmData[index] >= 0 ? (parent.height / 2) - height : (parent.height / 2)
                                        color: backend.pwmData[index] >= 0 ? accent : alertColor // Opcional: rojo para negativos
                                        opacity: 0.8
                                        Behavior on height { NumberAnimation { duration: 150 } }
                                        Behavior on y { NumberAnimation { duration: 150 } }
                                    }
                                }
                                Text {
                                    text: backend.pwmData[index] !== undefined ? backend.pwmData[index] : "0"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: -20
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    color: textColor; font.family: monoFont; font.pixelSize: 9
                                }
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: borderColor; Layout.topMargin: 15; Layout.bottomMargin: 15 }

                    // Gráficas de Ángulos (360 grados)
                    Text { text: "ANGULO DE ARTICULACIONES [0° a 360°]"; color: textDim; font.family: monoFont.name; font.pixelSize: 9; font.letterSpacing: 2 }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 180
                        spacing: 10
                        Repeater {
                            model: 4
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 15; height: parent.height
                                    color: "transparent"
                                    border.color: borderColor
                                    Rectangle {
                                        width: parent.width - 2
                                        height: (backend.anglesData[index] / 360) * (parent.height - 2)
                                        anchors.bottom: parent.bottom
                                        anchors.bottomMargin: 1
                                        anchors.horizontalCenter: parent.horizontalCenter
                                        color: "#ffffff"
                                        opacity: 0.6
                                        Behavior on height { NumberAnimation { duration: 150 } }
                                    }
                                }
                                Text {
                                    text: (backend.anglesData[index] || "0") + "°"
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: -20
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    color: textColor; font.family: monoFont.name; font.pixelSize: 9
                                }
                            }
                        }
                    }
                    Item { Layout.fillHeight: true }

                }
            }
        }

        // ================= PIE DE PÁGINA =================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "transparent"
            border.color: borderColor

            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 20
                text: "ESTADO: " + backend.portStatus
                color: backend.portStatus === "DESCONECTADO" ? alertColor : accent
                font.family: monoFont.name
                font.pixelSize: 11
                font.letterSpacing: 2
            }

            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 20
                text: "PROYECTO INTEGRADOR DE COMPETENCIAS 2"
                color: textDim
                font.family: monoFont.name
                font.pixelSize: 11
            }
        }
    }
}