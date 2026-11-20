import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

Window {
    id: mainWindow
    width: 1360
    height: 760
    minimumWidth: 1180
    minimumHeight: 680
    visible: true
    title: "PIC 2 — Monitoreo y calibración NVS"
    color: bgApp

    // ---------- Paleta sobria / minimalista ----------
    property color bgApp: "#111316"
    property color bgPanel: "#181B20"
    property color bgInset: "#121519"
    property color bgHover: "#222730"
    property color borderColor: "#282D37"
    property color textColor: "#E4E7EC"
    property color textDim: "#8A93A2"
    property color neutral: "#B8C0CC"
    property color accent: "#7895B2"
    property color alertColor: "#C96A6A"

    property color stateActiveColor: "#6BA388"
    property color stateWarnColor: "#C89B5B"
    property color stateBlueColor: "#7895B2"
    property color stateIdleColor: "#6B7280"

    // ---------- Tipografía y estado ----------
    property string uiFont: "Segoe UI"
    property string monoFont: "Consolas"
    property int rightTabIndex: 0
    property string lastSentCmd: ""
    property bool logCleared: false

    Connections {
        target: backend
        function onSerialLogChanged() {
            if (mainWindow.logCleared && backend.serialLog.length > 0) {
                mainWindow.logCleared = false
            }
        }
    }

    // ======================================================
    //  COMPONENTES REUTILIZABLES
    // ======================================================

    component SectionTitle: Text {
        color: textColor
        font.family: uiFont
        font.pixelSize: 12
        font.weight: Font.DemiBold
    }

    component Divider: Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        Layout.topMargin: 4
        Layout.bottomMargin: 4
        color: borderColor
    }

    component FieldLabel: Text {
        color: textDim
        font.family: uiFont
        font.pixelSize: 11
    }

    // Indicador de estado sobrio
    component StatusPill: Rectangle {
        id: pillRoot
        property string valorTexto: "Quieto"
        property color estadoColor: stateIdleColor

        function limpiar(s) {
            var t = String(s).replace(/[\[\]]/g, "").trim().toLowerCase()
            return t.charAt(0).toUpperCase() + t.slice(1)
        }

        Layout.alignment: Qt.AlignRight
        implicitWidth: row.implicitWidth + 18
        implicitHeight: 22
        radius: 4
        color: Qt.rgba(estadoColor.r, estadoColor.g, estadoColor.b, 0.10)
        border.width: 1
        border.color: Qt.rgba(estadoColor.r, estadoColor.g, estadoColor.b, 0.30)

        Row {
            id: row
            anchors.centerIn: parent
            spacing: 6
            Rectangle {
                width: 6; height: 6; radius: 3
                anchors.verticalCenter: parent.verticalCenter
                color: pillRoot.estadoColor
            }
            Text {
                text: pillRoot.limpiar(pillRoot.valorTexto)
                color: pillRoot.estadoColor
                font.family: uiFont
                font.pixelSize: 11
                font.weight: Font.Medium
            }
        }
    }

    component AppField: TextField {
        id: fieldRoot
        color: textColor
        placeholderTextColor: "#5A6270"
        selectionColor: accent
        selectedTextColor: "#111316"
        font.family: monoFont
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        leftPadding: 6
        rightPadding: 6
        topPadding: 0
        bottomPadding: 0
        implicitHeight: 28
        selectByMouse: true
        background: Rectangle {
            radius: 4
            color: bgInset
            border.width: 1
            border.color: fieldRoot.activeFocus ? accent : borderColor
        }
    }

    // variant: "filled" (acción principal), "tonal" (por defecto), "outline"
    component AppButton: Button {
        id: btnRoot
        property color btnColor: neutral
        property string variant: "tonal"

        implicitHeight: 30
        leftPadding: 12
        rightPadding: 12
        hoverEnabled: true

        contentItem: Text {
            text: btnRoot.text
            color: btnRoot.variant === "filled" ? "#111316" : btnRoot.btnColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            font.family: uiFont
            font.pixelSize: 12
            font.weight: Font.DemiBold
        }
        background: Rectangle {
            radius: 4
            color: {
                var c = btnRoot.btnColor
                if (btnRoot.variant === "filled")
                    return btnRoot.pressed ? Qt.darker(c, 1.15) : (btnRoot.hovered ? Qt.lighter(c, 1.08) : c)
                if (btnRoot.variant === "outline")
                    return btnRoot.pressed ? Qt.rgba(c.r, c.g, c.b, 0.14) : (btnRoot.hovered ? Qt.rgba(c.r, c.g, c.b, 0.07) : "transparent")
                return Qt.rgba(c.r, c.g, c.b, btnRoot.pressed ? 0.22 : (btnRoot.hovered ? 0.16 : 0.10))
            }
            border.width: btnRoot.variant === "filled" ? 0 : 1
            border.color: Qt.rgba(btnRoot.btnColor.r, btnRoot.btnColor.g, btnRoot.btnColor.b, 0.35)
        }
    }

    // Pestaña minimalista con subrayado fino
    component TabItem: Item {
        id: tabRoot
        property string label: ""
        property bool selected: false
        signal clicked()

        Layout.fillWidth: true
        implicitHeight: 34

        Text {
            anchors.centerIn: parent
            text: tabRoot.label
            color: tabRoot.selected ? textColor : textDim
            font.family: uiFont
            font.pixelSize: 12
            font.weight: tabRoot.selected ? Font.DemiBold : Font.Normal
        }
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 2
            color: tabRoot.selected ? accent : "transparent"
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: tabRoot.clicked()
        }
    }

    // Barra vertical sobria (PWM bipolar o ángulo)
    component BarGauge: ColumnLayout {
        id: gaugeRoot
        property string etiqueta: ""
        property string valorTexto: "0"
        property real valor: 0
        property real minimo: 0
        property real maximo: 100
        property bool bipolar: false

        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 4

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                id: track
                anchors.horizontalCenter: parent.horizontalCenter
                width: 14
                height: parent.height
                radius: 2
                color: bgInset
                border.color: borderColor

                // Línea de cero
                Rectangle {
                    visible: gaugeRoot.bipolar
                    width: parent.width; height: 1
                    anchors.verticalCenter: parent.verticalCenter
                    color: textDim
                }

                Rectangle {
                    id: fill
                    property real mitad: (track.height - 2) / 2
                    property real frac: gaugeRoot.bipolar
                        ? Math.min(1, Math.abs(gaugeRoot.valor) / gaugeRoot.maximo)
                        : Math.max(0, Math.min(1, (gaugeRoot.valor - gaugeRoot.minimo) / (gaugeRoot.maximo - gaugeRoot.minimo)))
                    width: track.width - 4
                    anchors.horizontalCenter: parent.horizontalCenter
                    radius: 1.5
                    height: gaugeRoot.bipolar ? frac * mitad : frac * (track.height - 4)
                    y: gaugeRoot.bipolar
                        ? (gaugeRoot.valor >= 0 ? track.height / 2 - height : track.height / 2)
                        : track.height - 2 - height
                    color: gaugeRoot.bipolar ? (gaugeRoot.valor >= 0 ? accent : alertColor) : stateBlueColor
                    Behavior on height { NumberAnimation { duration: 100 } }
                    Behavior on y { NumberAnimation { duration: 100 } }
                }
            }
        }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: gaugeRoot.valorTexto
            color: textColor
            font.family: monoFont
            font.pixelSize: 11
        }
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: gaugeRoot.etiqueta
            color: textDim
            font.family: uiFont
            font.pixelSize: 10
        }
    }

    // Slider sobrio
    component AppSlider: Slider {
        id: sRoot
        implicitHeight: 22
        background: Rectangle {
            x: sRoot.leftPadding
            y: sRoot.topPadding + sRoot.availableHeight / 2 - height / 2
            width: sRoot.availableWidth
            height: 3
            radius: 1.5
            color: borderColor
            Rectangle {
                width: sRoot.visualPosition * parent.width
                height: parent.height
                radius: 1.5
                color: accent
            }
        }
        handle: Rectangle {
            x: sRoot.leftPadding + sRoot.visualPosition * (sRoot.availableWidth - width)
            y: sRoot.topPadding + sRoot.availableHeight / 2 - height / 2
            width: 12; height: 12; radius: 6
            color: sRoot.pressed ? "#FFFFFF" : neutral
            border.width: 1
            border.color: bgApp
        }
    }

    // ======================================================
    //  LAYOUT PRINCIPAL
    // ======================================================
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        // ================= CABECERA =================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 46
            radius: 5
            color: bgPanel
            border.color: borderColor

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 12

                Column {
                    spacing: 1
                    Text {
                        text: "Monitoreo de cadena cinemática"
                        color: textColor
                        font.family: uiFont
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }
                    Text {
                        text: "Calibración y control del brazo robótico"
                        color: textDim
                        font.family: uiFont
                        font.pixelSize: 11
                    }
                }

                Item { Layout.fillWidth: true }

                Column {
                    spacing: 1
                    Text {
                        anchors.right: parent.right
                        text: "Proyecto de"
                        color: textDim
                        font.family: uiFont
                        font.pixelSize: 10
                    }
                    Text {
                        anchors.right: parent.right
                        text: "David Álvarez, Damián Bonilla, Matías Hernández"
                        color: neutral
                        font.family: uiFont
                        font.pixelSize: 11
                    }
                }
            }
        }

        // ================= CUERPO =================
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            // --- PANEL IZQUIERDO (Conexión, ejecución y sensores) ---
            Rectangle {
                Layout.preferredWidth: 295
                Layout.fillHeight: true
                radius: 5
                color: bgPanel
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    SectionTitle { text: "Conexión serial" }

                    RowLayout {
                        spacing: 6
                        ComboBox {
                            id: portCombo
                            model: backend.availablePorts
                            editable: true
                            Layout.fillWidth: true
                            implicitHeight: 30
                            font.family: monoFont
                            font.pixelSize: 12
                            palette.text: textColor
                            palette.highlight: accent

                            background: Rectangle {
                                radius: 4
                                color: bgInset
                                border.width: 1
                                border.color: portCombo.activeFocus ? accent : borderColor
                            }
                            indicator: Text {
                                x: portCombo.width - width - 10
                                y: (portCombo.height - height) / 2
                                text: "▾"
                                color: textDim
                                font.pixelSize: 12
                            }
                            delegate: ItemDelegate {
                                width: portCombo.width
                                highlighted: portCombo.highlightedIndex === index
                                contentItem: Text {
                                    text: modelData
                                    color: textColor
                                    font.family: monoFont
                                    font.pixelSize: 12
                                    verticalAlignment: Text.AlignVCenter
                                }
                                background: Rectangle {
                                    color: parent.highlighted || parent.hovered ? bgHover : "transparent"
                                }
                            }
                            popup: Popup {
                                y: portCombo.height + 2
                                width: portCombo.width
                                implicitHeight: Math.min(contentItem.implicitHeight + 2, 180)
                                padding: 1
                                contentItem: ListView {
                                    clip: true
                                    implicitHeight: contentHeight
                                    model: portCombo.popup.visible ? portCombo.delegateModel : null
                                    currentIndex: portCombo.highlightedIndex
                                    ScrollIndicator.vertical: ScrollIndicator {}
                                }
                                background: Rectangle {
                                    radius: 4
                                    color: bgPanel
                                    border.color: borderColor
                                }
                            }
                        }
                        AppButton {
                            text: "↻"
                            btnColor: textDim
                            variant: "outline"
                            Layout.preferredWidth: 32
                            leftPadding: 0
                            rightPadding: 0
                            onClicked: backend.refresh_ports()
                        }
                        AppField {
                            id: baudInput
                            text: "115200"
                            Layout.preferredWidth: 68
                            implicitHeight: 30
                        }
                    }

                    AppButton {
                        text: "Conectar"
                        btnColor: accent
                        variant: "filled"
                        Layout.fillWidth: true
                        onClicked: backend.connect_serial(portCombo.editText, baudInput.text)
                    }

                    Divider {}

                    SectionTitle { text: "Ejecución del sistema" }

                    RowLayout {
                        spacing: 8
                        AppButton {
                            text: "Iniciar"
                            btnColor: stateActiveColor
                            Layout.fillWidth: true
                            onClicked: backend.start_system()
                        }
                        AppButton {
                            text: "Parar"
                            btnColor: alertColor
                            Layout.fillWidth: true
                            onClicked: backend.stop_system()
                        }
                    }

                    Divider {}

                    SectionTitle { text: "Sensores y actuadores" }

                    GridLayout {
                        columns: 2
                        columnSpacing: 8
                        rowSpacing: 10
                        Layout.fillWidth: true

                        FieldLabel { text: "Cinta 1"; Layout.fillWidth: true }
                        StatusPill {
                            valorTexto: backend.conveyor1 ? "Activo" : "Parado"
                            estadoColor: backend.conveyor1 ? stateActiveColor : stateIdleColor
                        }

                        FieldLabel { text: "Cinta 2"; Layout.fillWidth: true }
                        StatusPill {
                            valorTexto: backend.conveyor2 ? "Activo" : "Parado"
                            estadoColor: backend.conveyor2 ? stateActiveColor : stateIdleColor
                        }

                        FieldLabel { text: "Pistón"; Layout.fillWidth: true }
                        StatusPill {
                            valorTexto: backend.servo1State
                            estadoColor: {
                                var s = backend.servo1State.toUpperCase().replace(/[\[\]\s]/g, "")
                                return s === "QUIETO" ? stateIdleColor : stateWarnColor
                            }
                        }

                        FieldLabel { text: "Ultrasónico 1"; Layout.fillWidth: true }
                        StatusPill {
                            valorTexto: backend.ultra1 ? "Detectado" : "Vacío"
                            estadoColor: backend.ultra1 ? stateWarnColor : stateIdleColor
                        }

                        FieldLabel { text: "Ultrasónico 2"; Layout.fillWidth: true }
                        StatusPill {
                            valorTexto: backend.ultra2 ? "Detectado" : "Vacío"
                            estadoColor: backend.ultra2 ? stateWarnColor : stateIdleColor
                        }

                        FieldLabel { text: "Color del palet"; Layout.fillWidth: true }
                        StatusPill {
                            valorTexto: backend.palletColor
                            estadoColor: {
                                var c = backend.palletColor.toUpperCase()
                                if (c.indexOf("ROJO") !== -1) return alertColor
                                if (c.indexOf("AMARILLO") !== -1) return stateWarnColor
                                if (c.indexOf("AZUL") !== -1) return stateBlueColor
                                return stateIdleColor
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // --- COLUMNA CENTRAL (Gemelo digital 3D + Sección Terminal Serial) ---
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 10

                // 1. Visor 3D
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 5
                    color: "#0E1013"
                    border.color: borderColor
                    clip: true

                    View3D {
                        id: view3D
                        anchors.fill: parent
                        anchors.margins: 1

                        environment: SceneEnvironment {
                            clearColor: "#0E1013"
                            backgroundMode: SceneEnvironment.Color
                            antialiasingMode: SceneEnvironment.MSAA
                            lightProbe: Texture { source: "images/studio_hdr.hdr" }
                            probeExposure: 1.3
                        }
                        PerspectiveCamera {
                            id: camera
                            position: Qt.vector3d(0, 20, 50)
                            eulerRotation.x: -3
                        }
                        DirectionalLight { eulerRotation.x: 30; eulerRotation.y: 180; brightness: 2.6; color: "#ffffff"; castsShadow: false }
                        DirectionalLight { eulerRotation.x: -30; eulerRotation.y: -45; brightness: 1.1; castsShadow: false }
                        DirectionalLight { eulerRotation.x: 30; eulerRotation.y: 135; brightness: 0.7; castsShadow: false }
                        DirectionalLight { eulerRotation.x: -90; brightness: 0.4; castsShadow: false }

                        OrbitCameraController { origin: brazo; camera: camera }

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

                    Text {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: 12
                        text: "Gemelo digital"
                        color: textDim
                        font.family: uiFont
                        font.pixelSize: 11
                        font.weight: Font.Medium
                    }

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.margins: 12
                        text: "Arrastrar para orbitar · Rueda para acercar"
                        color: "#5A6270"
                        font.family: uiFont
                        font.pixelSize: 10
                    }
                }

                // 2. Sección dedicada a Terminal Serial (amplia y en la misma ventana)
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 205
                    radius: 5
                    color: bgPanel
                    border.color: borderColor

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        // Cabecera de la terminal con acciones rápidas
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            SectionTitle { text: "Terminal serial"; Layout.fillWidth: true }

                            AppButton {
                                text: "STATUS"
                                btnColor: neutral
                                variant: "outline"
                                implicitHeight: 24
                                leftPadding: 8; rightPadding: 8
                                onClicked: backend.send_raw_command("STATUS")
                            }
                            AppButton {
                                text: "GET_CFG"
                                btnColor: neutral
                                variant: "outline"
                                implicitHeight: 24
                                leftPadding: 8; rightPadding: 8
                                onClicked: backend.request_config()
                            }
                            AppButton {
                                text: "Limpiar"
                                btnColor: textDim
                                variant: "outline"
                                implicitHeight: 24
                                leftPadding: 8; rightPadding: 8
                                onClicked: mainWindow.logCleared = true
                            }
                        }

                        // Consola de salida
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 4
                            color: bgInset
                            border.color: borderColor
                            clip: true

                            ScrollView {
                                anchors.fill: parent
                                anchors.margins: 8

                                TextArea {
                                    id: cliOutput
                                    text: mainWindow.logCleared
                                          ? ""
                                          : (backend.serialLog.length > 0 ? backend.serialLog : "Esperando conexión serial...")
                                    color: backend.serialLog.length > 0 && !mainWindow.logCleared ? "#D1D5DB" : textDim
                                    font.family: monoFont
                                    font.pixelSize: 12
                                    readOnly: true
                                    selectByMouse: true
                                    wrapMode: Text.WrapAnywhere
                                    background: null
                                    onTextChanged: cursorPosition = text.length
                                }
                            }
                        }

                        // Línea de comandos (CLI)
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            AppField {
                                id: cliInput
                                placeholderText: "Comando (ej: STATUS, HOME, ANG 1 30, GET_CFG)..."
                                horizontalAlignment: Text.AlignLeft
                                implicitHeight: 28
                                Layout.fillWidth: true

                                Keys.onUpPressed: {
                                    if (mainWindow.lastSentCmd.length > 0) {
                                        text = mainWindow.lastSentCmd
                                    }
                                }
                                onAccepted: {
                                    if (text.trim().length > 0) {
                                        mainWindow.lastSentCmd = text.trim()
                                        backend.send_raw_command(text)
                                        text = ""
                                    }
                                }
                            }
                            AppButton {
                                text: "Enviar"
                                btnColor: accent
                                variant: "filled"
                                implicitHeight: 28
                                onClicked: {
                                    if (cliInput.text.trim().length > 0) {
                                        mainWindow.lastSentCmd = cliInput.text.trim()
                                        backend.send_raw_command(cliInput.text)
                                        cliInput.text = ""
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // --- PANEL DERECHO (telemetría / parámetros NVS) ---
            Rectangle {
                Layout.preferredWidth: 350
                Layout.fillHeight: true
                radius: 5
                color: bgPanel
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    // Barra de pestañas
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 0
                            TabItem {
                                label: "Telemetría y pruebas"
                                selected: rightTabIndex === 0
                                onClicked: rightTabIndex = 0
                            }
                            TabItem {
                                label: "Parámetros NVS"
                                selected: rightTabIndex === 1
                                onClicked: rightTabIndex = 1
                            }
                        }
                        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: borderColor }
                    }

                    // ================= PESTAÑA 0: TELEMETRÍA Y CONTROL =================
                    ColumnLayout {
                        visible: rightTabIndex === 0
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            SectionTitle { text: "Señal PWM"; Layout.fillWidth: true }
                            FieldLabel { text: "−100 a 100" }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 104
                            spacing: 8
                            Repeater {
                                model: ["Base", "Hombro", "Codo", "Muñeca"]
                                BarGauge {
                                    etiqueta: modelData
                                    bipolar: true
                                    maximo: 100
                                    valor: backend.pwmData[index] !== undefined ? backend.pwmData[index] : 0
                                    valorTexto: String(valor)
                                }
                            }
                        }

                        Divider {}

                        RowLayout {
                            Layout.fillWidth: true
                            SectionTitle { text: "Ángulo de articulaciones"; Layout.fillWidth: true }
                            FieldLabel { text: "0° a 360°" }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 104
                            spacing: 8
                            Repeater {
                                model: ["Base", "Hombro", "Codo", "Muñeca"]
                                BarGauge {
                                    etiqueta: modelData
                                    minimo: 0
                                    maximo: 360
                                    valor: backend.anglesData[index] !== undefined ? backend.anglesData[index] : 0
                                    valorTexto: String(valor) + "°"
                                }
                            }
                        }

                        Divider {}

                        SectionTitle { text: "Control rápido y pruebas" }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6
                            AppButton { text: "Home"; btnColor: stateBlueColor; Layout.fillWidth: true; onClicked: backend.home_arm() }
                            AppButton { text: "Hold"; btnColor: stateWarnColor; Layout.fillWidth: true; onClicked: backend.hold_arm() }
                            AppButton { text: "Status"; btnColor: neutral; variant: "outline"; Layout.fillWidth: true; onClicked: backend.send_raw_command("STATUS") }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 6
                            AppButton { text: "Test rojo"; btnColor: alertColor; Layout.fillWidth: true; onClicked: backend.run_color_test("ROJO") }
                            AppButton { text: "Test azul"; btnColor: stateBlueColor; Layout.fillWidth: true; onClicked: backend.run_color_test("AZUL") }
                            AppButton { text: "Test amarillo"; btnColor: stateWarnColor; Layout.fillWidth: true; onClicked: backend.run_color_test("AMARILLO") }
                        }

                        // Control manual por eje
                        Repeater {
                            model: [
                                { nombre: "Base", min: -15, max: 105 },
                                { nombre: "Hombro", min: -90, max: 90 },
                                { nombre: "Codo", min: -135, max: 45 },
                                { nombre: "Muñeca", min: -110, max: 110 }
                            ]
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                FieldLabel { text: modelData.nombre; Layout.preferredWidth: 52 }
                                AppSlider {
                                    id: axisSlider
                                    from: modelData.min
                                    to: modelData.max
                                    value: 0
                                    stepSize: 1
                                    Layout.fillWidth: true
                                    onMoved: backend.move_axis_deg(index, value)
                                }
                                Text {
                                    text: Math.round(axisSlider.value) + "°"
                                    color: textColor
                                    font.family: monoFont
                                    font.pixelSize: 11
                                    horizontalAlignment: Text.AlignRight
                                    Layout.preferredWidth: 40
                                }
                            }
                        }

                        Item { Layout.fillHeight: true }
                    }

                    // ================= PESTAÑA 1: PARÁMETROS NVS =================
                    ScrollView {
                        id: nvsScroll
                        visible: rightTabIndex === 1
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        ColumnLayout {
                            width: nvsScroll.availableWidth
                            spacing: 8

                            RowLayout {
                                Layout.fillWidth: true
                                SectionTitle { text: "Ganancias PID y gravedad"; Layout.fillWidth: true }
                                AppButton { text: "Leer ESP32"; btnColor: stateBlueColor; implicitHeight: 26; onClicked: backend.request_config() }
                            }

                            // Cabecera tabla PID
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 4
                                FieldLabel { text: "Eje"; Layout.preferredWidth: 52 }
                                FieldLabel { text: "Kp"; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                                FieldLabel { text: "Ki"; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                                FieldLabel { text: "Kd"; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                                FieldLabel { text: "Grav."; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                                Item { Layout.preferredWidth: 36 }
                            }

                            Repeater {
                                id: pidRepeater
                                model: ["Base", "Hombro", "Codo", "Muñeca"]
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    Text {
                                        text: modelData
                                        color: textColor
                                        font.family: uiFont
                                        font.pixelSize: 11
                                        Layout.preferredWidth: 52
                                    }
                                    AppField { id: kpF; text: backend.pidParams[index][0].toFixed(2); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                    AppField { id: kiF; text: backend.pidParams[index][1].toFixed(2); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                    AppField { id: kdF; text: backend.pidParams[index][2].toFixed(2); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                    AppField { id: grF; text: backend.pidParams[index][3].toFixed(1); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                    AppButton {
                                        text: "OK"
                                        btnColor: stateActiveColor
                                        implicitHeight: 28
                                        leftPadding: 0
                                        rightPadding: 0
                                        Layout.preferredWidth: 36
                                        onClicked: backend.apply_axis_pid(index, parseFloat(kpF.text), parseFloat(kiF.text), parseFloat(kdF.text), parseFloat(grF.text))
                                    }
                                }
                            }

                            Divider {}

                            SectionTitle { text: "Coordenadas pick and place (m o cm)" }

                            GridLayout {
                                columns: 4
                                columnSpacing: 4
                                rowSpacing: 6
                                Layout.fillWidth: true

                                FieldLabel { text: "Pick X / Y / Z"; Layout.preferredWidth: 78 }
                                AppField { id: pxF; text: backend.posParams.pickX.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: pyF; text: backend.posParams.pickY.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: pzF; text: backend.posParams.pickZ.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }

                                FieldLabel { text: "Cont. X / Y / dz"; Layout.preferredWidth: 78 }
                                AppField { id: cxF; text: backend.posParams.contX.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: cyF; text: backend.posParams.contY.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: liftF; text: backend.posParams.liftZ.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }

                                FieldLabel { text: "Z rojo / azul / amar."; Layout.preferredWidth: 78; elide: Text.ElideRight }
                                AppField { id: zrF; text: backend.posParams.zRojo.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: zaF; text: backend.posParams.zAzul.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: zamF; text: backend.posParams.zAmarillo.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }

                                FieldLabel { text: "Avance / bajada"; Layout.preferredWidth: 78 }
                                AppField { id: avF; text: backend.posParams.avance.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: bajF; text: backend.posParams.bajada.toFixed(3); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppButton {
                                    text: "Aplicar"
                                    btnColor: stateActiveColor
                                    implicitHeight: 28
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: 1
                                    onClicked: backend.apply_positions(
                                        parseFloat(pxF.text), parseFloat(pyF.text), parseFloat(pzF.text),
                                        parseFloat(liftF.text), parseFloat(cxF.text), parseFloat(cyF.text),
                                        parseFloat(zrF.text), parseFloat(zaF.text), parseFloat(zamF.text),
                                        parseFloat(avF.text), parseFloat(bajF.text)
                                    )
                                }
                            }

                            Divider {}

                            SectionTitle { text: "Tiempos del pistón (ms) y velocidad de cinta" }

                            GridLayout {
                                columns: 4
                                columnSpacing: 4
                                rowSpacing: 6
                                Layout.fillWidth: true

                                FieldLabel { text: "Retardo / ext. / ret."; Layout.preferredWidth: 78; elide: Text.ElideRight }
                                AppField { id: pRetardoF; text: backend.posParams.pistonRetardo; Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: pExtF; text: backend.posParams.pistonExt; Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppField { id: pRetF; text: backend.posParams.pistonRet; Layout.fillWidth: true; Layout.preferredWidth: 1 }

                                FieldLabel { text: "Vel. cinta"; Layout.preferredWidth: 78 }
                                AppField { id: velCintaF; text: backend.posParams.velCinta.toFixed(0); Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                Item { Layout.fillWidth: true; Layout.preferredWidth: 1 }
                                AppButton {
                                    text: "Aplicar"
                                    btnColor: stateActiveColor
                                    implicitHeight: 28
                                    Layout.fillWidth: true
                                    Layout.preferredWidth: 1
                                    onClicked: backend.apply_transport_cfg(
                                        parseInt(pRetardoF.text), parseInt(pExtF.text),
                                        parseInt(pRetF.text), parseFloat(velCintaF.text)
                                    )
                                }
                            }

                            Divider {}

                            // Zona de memoria no volátil sobria
                            Rectangle {
                                Layout.fillWidth: true
                                implicitHeight: nvsCol.implicitHeight + 22
                                radius: 5
                                color: bgInset
                                border.color: borderColor

                                ColumnLayout {
                                    id: nvsCol
                                    anchors.fill: parent
                                    anchors.margins: 11
                                    spacing: 8

                                    SectionTitle { text: "Memoria no volátil (flash NVS)" }
                                    FieldLabel {
                                        text: "Guardar conserva los parámetros al reiniciar. Fábrica restaura los valores originales."
                                        wrapMode: Text.WordWrap
                                        Layout.fillWidth: true
                                    }
                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        AppButton {
                                            text: "Guardar en NVS"
                                            btnColor: stateActiveColor
                                            variant: "filled"
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: 1
                                            onClicked: backend.save_to_nvs()
                                        }
                                        AppButton {
                                            text: "Restaurar fábrica"
                                            btnColor: alertColor
                                            variant: "outline"
                                            Layout.fillWidth: true
                                            Layout.preferredWidth: 1
                                            onClicked: backend.reset_nvs()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // ================= PIE DE PÁGINA =================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            radius: 5
            color: bgPanel
            border.color: borderColor

            property bool okState: backend.portStatus.indexOf("CONECTADO") === 0 ||
                                   backend.portStatus.indexOf("SISTEMA ACTIVO") === 0 ||
                                   backend.portStatus.indexOf("NVS: PARÁMETROS") === 0

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 8

                Rectangle {
                    width: 7; height: 7; radius: 3.5
                    color: parent.parent.okState ? stateActiveColor : alertColor
                }
                Text {
                    text: backend.portStatus
                    color: parent.parent.okState ? stateActiveColor : textDim
                    font.family: uiFont
                    font.pixelSize: 11
                    font.weight: Font.Medium
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: "Proyecto Integrador de Competencias 2"
                    color: textDim
                    font.family: uiFont
                    font.pixelSize: 11
                }
            }
        }
    }
}