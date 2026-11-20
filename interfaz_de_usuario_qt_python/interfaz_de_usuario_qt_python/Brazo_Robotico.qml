import QtQuick
import QtQuick3D

Node {
    id: root
    objectName: "Root"

    // =========================================================
    // PROPIEDADES CONECTADAS A MAIN.QML / BACKEND
    // =========================================================
    property real anguloBase: 0
    property real anguloHombro: 0
    property real anguloCodo: 0
    property real anguloGarra: 0

    // =========================================================
    // CALIBRACIÓN DEL CERO (0, 0, 0, 0) Y SENTIDO DE GIRO
    // =========================================================
    property real offsetBase: 0
    property real offsetHombro: 0
    property real offsetCodo: 0
    property real offsetGarra: 0

    property real dirBase: 1
    property real dirHombro: 1
    property real dirCodo: 1
    property real dirGarra: 1

    // =========================================================
    // PIVOTES EXACTOS EXTRAÍDOS DEL ENSAMBLAJE
    // =========================================================
    readonly property vector3d pivoteBase:   Qt.vector3d(-0.00565, 0.0, 0.0175)
    readonly property vector3d pivoteHombro: Qt.vector3d(-0.00565, -0.007, 0.0818)
    readonly property vector3d pivoteCodo:   Qt.vector3d(-0.00565, 0.0095, 0.18815)
    readonly property vector3d pivoteMuneca: Qt.vector3d(-0.00565, -0.006, 0.2975)

    // =========================================================
    // MATERIALES ORIGINALES DEL ENSAMBLAJE
    // =========================================================
    PrincipledMaterial { id: matTurquesa;   baseColor: "#ffc1eeea"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matVioleta;    baseColor: "#ffe2c3e7"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matGrisOscuro; baseColor: "#ff8c9195"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matGrisClaro;  baseColor: "#ffd2d2d2"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matHerram;     baseColor: "#ffe2ecf6"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matNaranja;    baseColor: "#fff0b299"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matVerde;      baseColor: "#ffd3eac8"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matCeleste;    baseColor: "#ffcee8f7"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matDorado;     baseColor: "#fff5ddba"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matNegro;      baseColor: "#ff959595"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matCrema;      baseColor: "#fff8f0d7"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matBlanco;     baseColor: "#fffcfcfc"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }
    PrincipledMaterial { id: matPlomo;      baseColor: "#fff4f4f4"; roughness: 1; cullMode: PrincipledMaterial.NoCulling }

    // Lista reutilizable para los sensores AS5600 (109 sub-mallas)
    readonly property var matsAS5600: [
        matPlomo, matPlomo, matBlanco, matCrema, matCrema, matCrema, matCrema, matCrema, matCrema, matCrema,
        matCrema, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matCrema, matCrema, matCrema,
        matCrema, matCrema, matCrema, matCrema, matCrema, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo,
        matPlomo, matBlanco, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo,
        matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo,
        matPlomo, matPlomo, matNegro, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo,
        matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo, matPlomo,
        matPlomo, matPlomo, matPlomo, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro,
        matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro,
        matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matNegro, matDorado,
        matDorado, matDorado, matDorado, matDorado, matDorado, matDorado, matDorado, matDorado, matDorado
    ]

    Node {
        id: assembly_1
        objectName: "Assembly 1"

        // =========================================================
        // 1. ESLABÓN FIJO: BASE
        // =========================================================
        Node {
            id: link_base

            Node {
                objectName: "occurrence of Base"
                Model {
                    source: "meshes/base_mesh.mesh"
                    materials: Array(38).fill(matGrisClaro)
                }
            }
            Node {
                objectName: "occurrence of Part 1 (Base turquesa)"
                position: Qt.vector3d(0.0285, 3.46945e-18, 0.017)
                rotation: Qt.quaternion(0.5, 0.5, -0.5, -0.5)
                Model {
                    source: "meshes/part_1_mesh47.mesh"
                    materials: Array(16).fill(matTurquesa)
                }
            }
            Node {
                objectName: "occurrence of Part 1 (Sensor Base)"
                position: Qt.vector3d(-0.00565, 0, 0.0255)
                rotation: Qt.quaternion(-7.85046e-17, 0.707107, -0.707107, -7.85046e-17)
                Model {
                    source: "meshes/part_1_mesh65.mesh"
                    materials: root.matsAS5600
                }
            }
            Node {
                objectName: "occurrence of mg90s servo (Base)"
                position: Qt.vector3d(0.0351, -0.0003, -0.058)
                rotation: Qt.quaternion(-7.22801e-17, 0, 0, 1)
                Model {
                    source: "meshes/mg90s_servo_mesh32.mesh"
                    materials: Array(47).fill(matCeleste)
                }
            }

            // =========================================================
            // 2. ARTICULACIÓN 1: BASE (Gira en eje Z)
            // =========================================================
            Node {
                id: joint_Base
                position: root.pivoteBase
                eulerRotation.z: (root.anguloBase * root.dirBase) + root.offsetBase

                Node {
                    position: Qt.vector3d(-root.pivoteBase.x, -root.pivoteBase.y, -root.pivoteBase.z)

                    Node {
                        objectName: "occurrence of Acople (Base)"
                        position: Qt.vector3d(-0.00565, -4.17418e-18, 0.0175)
                        rotation: Qt.quaternion(1, 0, 0, 7.22801e-17)
                        Model {
                            source: "meshes/acople_mesh56.mesh"
                            materials: Array(24).fill(matGrisOscuro)
                        }
                    }
                    Node {
                        objectName: "occurrence of Part 1 (Acople verde)"
                        position: Qt.vector3d(-0.00565, -2.23129e-16, 0.0185)
                        Model {
                            source: "meshes/part_1_mesh21.mesh"
                            materials: Array(55).fill(matVerde)
                        }
                    }
                    Node {
                        objectName: "occurrence of MG996R Servo"
                        position: Qt.vector3d(-0.00565, -0.0486, 0.0716)
                        rotation: Qt.quaternion(-0.5, 0.5, -0.5, -0.5)
                        Model {
                            source: "meshes/mg996R_Servo_mesh.mesh"
                            materials: Array(79).fill(matGrisClaro)
                        }
                    }
                    Node {
                        objectName: "occurrence of Part 1 (Sensor Hombro)"
                        position: Qt.vector3d(-0.00566339, -0.0540068, 0.0817463)
                        rotation: Qt.quaternion(0.5, 0.5, -0.5, 0.5)
                        Model {
                            source: "meshes/part_1_mesh62.mesh"
                            materials: root.matsAS5600
                        }
                    }

                    // =========================================================
                    // 3. ARTICULACIÓN 2: HOMBRO (Gira en eje Y)
                    // =========================================================
                    Node {
                        id: joint_Hombro
                        position: root.pivoteHombro
                        eulerRotation.y: (root.anguloHombro * root.dirHombro) + root.offsetHombro

                        Node {
                            position: Qt.vector3d(-root.pivoteHombro.x, -root.pivoteHombro.y, -root.pivoteHombro.z)

                            Node {
                                objectName: "occurrence of Eslabon 2 (Hombro)"
                                position: Qt.vector3d(-0.00565, -0.007, 0.0818)
                                rotation: Qt.quaternion(0.707107, 3.08948e-17, 2.96682e-17, 0.707107)
                                Model {
                                    source: "meshes/eslabon_2_mesh40.mesh"
                                    materials: Array(58).fill(matVioleta)
                                }
                            }
                            Node {
                                objectName: "occurrence of mg90s servo (Hombro)"
                                position: Qt.vector3d(-0.00535, -0.066, 0.1474)
                                rotation: Qt.quaternion(-0.5, 0.5, 0.5, 0.5)
                                Model {
                                    source: "meshes/mg90s_servo_mesh59.mesh"
                                    materials: Array(47).fill(matCeleste)
                                }
                            }
                            Node {
                                objectName: "occurrence of Part 1 (Sensor Codo)"
                                position: Qt.vector3d(-0.00566339, 0.023, 0.187531)
                                rotation: Qt.quaternion(-0.5, -0.5, 0.5, -0.5)
                                Model {
                                    source: "meshes/part_1_mesh43.mesh"
                                    materials: root.matsAS5600
                                }
                            }

                            // =========================================================
                            // 4. ARTICULACIÓN 3: CODO (Gira en eje Y)
                            // =========================================================
                            Node {
                                id: joint_Codo
                                position: root.pivoteCodo
                                eulerRotation.y: (root.anguloCodo * root.dirCodo) + root.offsetCodo

                                Node {
                                    position: Qt.vector3d(-root.pivoteCodo.x, -root.pivoteCodo.y, -root.pivoteCodo.z)

                                    Node {
                                        objectName: "occurrence of Acople (Codo)"
                                        position: Qt.vector3d(-0.00565, 0.0095, 0.18815)
                                        rotation: Qt.quaternion(0.707107, -0.707107, 1.718e-16, -6.77938e-16)
                                        Model {
                                            source: "meshes/acople_mesh53.mesh"
                                            materials: Array(24).fill(matGrisOscuro)
                                        }
                                    }
                                    Node {
                                        objectName: "occurrence of Eslabon 2 (Antebrazo)"
                                        position: Qt.vector3d(-0.00565, 0.0105, 0.18815)
                                        rotation: Qt.quaternion(0.707107, -2.95843e-16, -1.05565e-15, 0.707107)
                                        Model {
                                            source: "meshes/eslabon_2_mesh.mesh"
                                            materials: Array(64).fill(matNaranja)
                                        }
                                    }
                                    Node {
                                        objectName: "occurrence of mg90s servo (Codo)"
                                        position: Qt.vector3d(-0.00595, 0.0715, 0.25675)
                                        rotation: Qt.quaternion(-0.5, -0.5, 0.5, -0.5)
                                        Model {
                                            source: "meshes/mg90s_servo_mesh.mesh"
                                            materials: Array(47).fill(matCeleste)
                                        }
                                    }
                                    Node {
                                        objectName: "occurrence of Part 1 (Sensor Muñeca)"
                                        position: Qt.vector3d(-0.00565, -0.0155, 0.298223)
                                        rotation: Qt.quaternion(0.5, -0.5, 0.5, 0.5)
                                        Model {
                                            source: "meshes/part_1_mesh.mesh"
                                            materials: root.matsAS5600
                                        }
                                    }

                                    // =========================================================
                                    // 5. ARTICULACIÓN 4: MUÑECA (Gira en eje Y)
                                    // =========================================================
                                    Node {
                                        id: joint_Muneca
                                        position: root.pivoteMuneca
                                        eulerRotation.y: (root.anguloGarra * root.dirGarra) + root.offsetGarra

                                        Node {
                                            position: Qt.vector3d(-root.pivoteMuneca.x, -root.pivoteMuneca.y, -root.pivoteMuneca.z)

                                            Node {
                                                objectName: "occurrence of Acople (Muñeca)"
                                                position: Qt.vector3d(-0.00565, -0.006, 0.2975)
                                                rotation: Qt.quaternion(0.703257, -0.703257, -0.0736871, -0.0736871)
                                                Model {
                                                    source: "meshes/acople_mesh.mesh"
                                                    materials: Array(24).fill(matGrisOscuro)
                                                }
                                            }
                                            Node {
                                                objectName: "occurrence of Herramienta"
                                                position: Qt.vector3d(-0.00565, -0.008, 0.2975)
                                                rotation: Qt.quaternion(0.703257, -0.0736871, -0.0736871, 0.703257)
                                                Model {
                                                    source: "meshes/herramienta_mesh.mesh"
                                                    materials: Array(53).fill(matHerram)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
