#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

#include <urdf/model.h>
#include <kdl_parser/kdl_parser.hpp>
#include <kdl/chain.hpp>
#include <kdl/tree.hpp>
#include <kdl/chainidsolver_recursive_newton_euler.hpp>
#include <kdl/chainfksolverpos_recursive.hpp> 
#include <kdl/jntarray.hpp>
#include <kdl/frames.hpp>

#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>

#include "Global_Config.h"
#include "Vectores.h"
#include "IK.h"

using namespace std::chrono_literals;

// =========================================================================
// ---------------------- CONFIGURACIÓN DE CARGA ÚTIL ----------------------
// =========================================================================
const double MASA_HERRAMIENTA_KG = 0.100; 

class NodoMapeadorTorque : public rclcpp::Node {
public:
    NodoMapeadorTorque() : Node("nodo_mapeador_torque") {
        joint_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
        timer_ = this->create_wall_timer(
            1s, std::bind(&NodoMapeadorTorque::ejecutar_mapeo_animado, this));
        RCLCPP_INFO(this->get_logger(), "Nodo mapeador inicializado. Aguardando conexión de RViz...");
    }

private:
    std::string obtener_texto_error_ik(ik_status_t status) {
        switch (status) {
            case IK_ERROR_BASE_FUERA_DE_LIMITE: return "Base fuera de limite angular";
            case IK_ERROR_SINGULARIDAD_MUNICA: return "Singularidad o zona de seguridad de la base";
            case IK_ERROR_FUERA_DE_ALCANCE: return "Punto fuera del alcance fisico del brazo";
            case IK_ERROR_SERVO_FUERA_DE_LIMITE: return "Angulo calculado excede limite de servomotor";
            case IK_ERROR_COLISION_MESA: return "Colision detectada con la mesa (Z < Z_MIN_MESA)";
            default: return "Error desconocido";
        }
    }

    void ejecutar_mapeo_animado() {
        timer_->cancel();

        RCLCPP_INFO(this->get_logger(), "Iniciando mapeo animado de torque basado en URDF...");
        RCLCPP_INFO(this->get_logger(), "Carga útil configurada: %.3f kg", MASA_HERRAMIENTA_KG);

        const char* home_dir = std::getenv("HOME");
        std::string ruta_csv = std::string(home_dir) + "/Proyectos/Proyecto-Integrador-de-Competencias-2/graficas/torques_plano_rz.csv";
        std::string ruta_urdf = ament_index_cpp::get_package_share_directory("descripcion_brazo") + "/urdf/robot.urdf";

        urdf::Model modelo_robot; 
        if (!modelo_robot.initFile(ruta_urdf)) {
            RCLCPP_ERROR(this->get_logger(), "No se pudo cargar el URDF en: %s", ruta_urdf.c_str());
            return;
        }

        KDL::Tree arbol_kdl;
        if (!kdl_parser::treeFromUrdfModel(modelo_robot, arbol_kdl)) {
            RCLCPP_ERROR(this->get_logger(), "Error al convertir URDF a árbol KDL");
            return;
        }

        KDL::Chain cadena_brazo;
        if (!arbol_kdl.getChain("base", "acople_4", cadena_brazo)) {
            RCLCPP_ERROR(this->get_logger(), "No se encontró la cadena cinemática KDL");
            return;
        }

        KDL::Vector gravedad(0.0, 0.0, -9.81);
        KDL::ChainIdSolver_RNE solucionador_dinamica(cadena_brazo, gravedad);
        KDL::ChainFkSolverPos_recursive solucionador_fk(cadena_brazo); 

        unsigned int num_joints = cadena_brazo.getNrOfJoints();
        KDL::JntArray q(num_joints), q_dot(num_joints), q_dotdot(num_joints), torques_salida(num_joints);
        KDL::Wrenches fuerzas_externas(cadena_brazo.getNrOfSegments(), KDL::Wrench::Zero());

        SetToZero(q_dot); 
        SetToZero(q_dotdot);

        std::ofstream archivo_csv(ruta_csv);
        archivo_csv << "R,Z,tau2_Nm,tau3_Nm,tau4_Nm\n";

        const double R_MIN = 0.050;
        const double R_MAX = 0.370;
        const double Z_MIN = 0.000;
        const double Z_MAX = 0.350;
        const double RESOLUCION = 0.001;

        sensor_msgs::msg::JointState joint_msg;
        joint_msg.name = {"Base", "Hombro", "Codo", "Muñeca"};

        int puntos_guardados = 0;
        int puntos_omitidos = 0;
        unsigned int ultimo_segmento = cadena_brazo.getNrOfSegments() - 1;

        for (double r = R_MIN; r <= R_MAX; r += RESOLUCION) {
            for (double z = Z_MIN; z <= Z_MAX; z += RESOLUCION) {
                vector_t destino = { static_cast<float>(r), 0.0f, static_cast<float>(z) };
                servomotores_h servos_out;

                ik_status_t st = IK_ext_debug(&servos_out, destino, nullptr);

                if (st == IK_OK) {
                    if (num_joints >= 4) {
                        q(0) = servos_out.sm1.angulo_objetivo; 
                        q(1) = servos_out.sm2.angulo_objetivo; 
                        q(2) = servos_out.sm3.angulo_objetivo; 
                        q(3) = servos_out.sm4.angulo_objetivo; 
                    }

                    // Calcular cinematica directa para obtener orientación
                    KDL::Frame pose_herramienta;
                    solucionador_fk.JntToCart(q, pose_herramienta);

                    // Definir el peso 
                    KDL::Vector peso_mundo(0.0, 0.0, -MASA_HERRAMIENTA_KG * 9.81);

                    // Rotar el peso al marco de referencia 
                    KDL::Vector peso_local = pose_herramienta.M.Inverse() * peso_mundo;

                    // Aplicar la fuerza transformada al ultimo segmento
                    fuerzas_externas[ultimo_segmento] = KDL::Wrench(peso_local, KDL::Vector::Zero());

                    joint_msg.header.stamp = this->now();
                    joint_msg.position = { q(0), q(1), q(2), q(3) };
                    joint_pub_->publish(joint_msg);

                    // Calcular dinamica 
                    if (solucionador_dinamica.CartToJnt(q, q_dot, q_dotdot, fuerzas_externas, torques_salida) >= 0) {
                        double tau_hombro = std::abs(torques_salida(1));
                        double tau_codo   = std::abs(torques_salida(2));
                        double tau_muneca = std::abs(torques_salida(3));

                        archivo_csv << r << "," << z << "," 
                                    << tau_hombro << "," 
                                    << tau_codo << "," 
                                    << tau_muneca << "\n";
                        puntos_guardados++;
                    }

                    rclcpp::sleep_for(1ms);
                } else {
                    puntos_omitidos++;
                    RCLCPP_WARN_THROTTLE(
                        this->get_logger(),
                        *this->get_clock(),
                        2000,
                        "Punto inalcanzable (R=%.3f, Z=%.3f) - Causa: %s",
                        r, z, obtener_texto_error_ik(st).c_str()
                    );
                }
            }
        }

        archivo_csv.close();
        RCLCPP_INFO(this->get_logger(), "Mapeo completado: %d guardados, %d omitidos.", puntos_guardados, puntos_omitidos);
    }

    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto nodo = std::make_shared<NodoMapeadorTorque>();
    rclcpp::spin(nodo);
    rclcpp::shutdown();
    return 0;
}
