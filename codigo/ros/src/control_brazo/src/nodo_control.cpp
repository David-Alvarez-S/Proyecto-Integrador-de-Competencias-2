#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"

#include "control_brazo/action/mover_brazo.hpp"

extern "C" {
#include "control_brazo/Aritmetica.h"
#include "control_brazo/Cuaterniones.h"
#include "control_brazo/Global_Config.h"
#include "control_brazo/Movimiento.h"
#include "control_brazo/PID.h"
#include "control_brazo/Perfil_S.h"
#include "control_brazo/Vectores.h"
}

using namespace std::chrono_literals;

// Período del lazo de control (s). Se usa para crear el timer.
#define CONTROL_DT_S 0.02f

class ControlBrazoNode : public rclcpp::Node {
public:
  using MoverBrazo = control_brazo::action::MoverBrazo;

  using GoalHandleMoverBrazo = rclcpp_action::ServerGoalHandle<MoverBrazo>;

  ControlBrazoNode() : Node("nodo_control_brazo") {
    // Publicador de articulaciones para RViz / Drivers
    publisher_ = this->create_publisher<sensor_msgs::msg::JointState>(
        "/joint_states", 10);

    // Crear Action Server
    action_server_ = rclcpp_action::create_server<MoverBrazo>(
        this, "mover_brazo",
        std::bind(&ControlBrazoNode::handle_goal, this, std::placeholders::_1,
                  std::placeholders::_2),
        std::bind(&ControlBrazoNode::handle_cancel, this,
                  std::placeholders::_1),
        std::bind(&ControlBrazoNode::handle_accepted, this,
                  std::placeholders::_1));

    // Pose inicial por defecto al encender. Llamada directa a IK_ext
    // (no es "un movimiento" entre dos puntos, es fijar el estado de
    // arranque), así que no pasa por Movimiento_Iniciar.
    vector_t v_inicial = {250.0f, 0.0f, 50.0f};
    if (!IK_ext(&servos_validos_, v_inicial, 0.0f, 0.0f, &continuidad_ik_,
                nullptr)) {
      RCLCPP_ERROR(this->get_logger(),
                   "Pose inicial inalcanzable: revisar Global_Config.h.");
    }

    // --- Control de velocidad (para cuando conectes los AS5600) ---
    // TODO: ajustar kp/ki/kd por servo (cada uno puede necesitar
    // ganancias distintas según carga/relación de transmisión).
    // TODO: limite_min/limite_max tienen que coincidir con lo que
    // espera tu driver del servo de giro continuo (p. ej. -1.0..1.0
    // normalizado, o un rango de pulso PWM en us con el "stop" en el
    // medio). Dejo -1..1 como placeholder.
    Iniciar_PID(&pid_sm1_, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f);
    Iniciar_PID(&pid_sm2_, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f);
    Iniciar_PID(&pid_sm3_, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f);
    Iniciar_PID(&pid_sm4_, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f);

    publisher_velocidades_ =
        this->create_publisher<std_msgs::msg::Float32MultiArray>(
            "/velocidades_servos", 10);

    // Bucle de control periódico (20ms == CONTROL_DT_S)
    timer_ = this->create_wall_timer(
        std::chrono::duration<float>(CONTROL_DT_S),
        std::bind(&ControlBrazoNode::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "Action Server 'mover_brazo' listo.");
  }

private:
  rclcpp_action::GoalResponse
  handle_goal(const rclcpp_action::GoalUUID &uuid,
              std::shared_ptr<const MoverBrazo::Goal> goal) {
    (void)uuid;
    RCLCPP_INFO(this->get_logger(),
                "Nueva meta recibida: X=%.2f, Y=%.2f, Z=%.2f", goal->x, goal->y,
                goal->z);

    vector_t v_meta = {goal->x, goal->y, goal->z};
    if (!Movimiento_Es_Alcanzable(v_meta)) {
      RCLCPP_WARN(this->get_logger(),
                  "Meta rechazada: (%.2f, %.2f, %.2f) fuera del espacio de "
                  "trabajo alcanzable.",
                  goal->x, goal->y, goal->z);
      return rclcpp_action::GoalResponse::REJECT;
    }

    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  // Se ejecuta si el cliente solicita cancelar la meta actual
  rclcpp_action::CancelResponse
  handle_cancel(const std::shared_ptr<GoalHandleMoverBrazo> goal_handle) {
    (void)goal_handle;
    RCLCPP_INFO(this->get_logger(), "Cancelación de movimiento solicitada.");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  // Se ejecuta al aceptar la meta para iniciar la nueva trayectoria
  void
  handle_accepted(const std::shared_ptr<GoalHandleMoverBrazo> goal_handle) {
    // Si ya había una meta ejecutándose, abortamos la anterior para priorizar
    // la nueva
    if (goal_handle_actual_ && goal_handle_actual_->is_active()) {
      auto result = std::make_shared<MoverBrazo::Result>();
      result->exito = false;
      result->mensaje = "Cancelado por un nuevo objetivo.";
      goal_handle_actual_->abort(result);
    }

    goal_handle_actual_ = goal_handle;
    const auto goal = goal_handle->get_goal();
    vector_t v_meta = {goal->x, goal->y, goal->z};
    limites_perfil_s_t limites = {V_MAX, A_MAX, J_MAX};

    if (!Movimiento_Iniciar(&movimiento_, servos_validos_, v_meta, limites,
                            &continuidad_ik_)) {

      RCLCPP_ERROR(this->get_logger(),
                   "IK falló pese a validación previa (bug o L_* "
                   "inconsistentes). Meta abortada.");
      auto result = std::make_shared<MoverBrazo::Result>();
      result->exito = false;
      result->mensaje = "Error interno de cinemática inversa.";
      goal_handle->abort(result);
      goal_handle_actual_.reset();
      return;
    }

    t_ref_ = this->now();
  }

  // --- BUCLE DE CONTROL DEL ROBOT ---
  void timer_callback() {
    if (movimiento_.activo) {
      const float t = static_cast<float>((this->now() - t_ref_).seconds());

      float progreso = 0.0f;
      bool en_curso =
          Movimiento_Evaluar(&servos_validos_, &movimiento_, t, &progreso);

      // Enviar Feedback al cliente que solicitó la acción
      if (goal_handle_actual_) {
        auto feedback = std::make_shared<MoverBrazo::Feedback>();
        feedback->progreso = progreso;
        goal_handle_actual_->publish_feedback(feedback);
      }

      if (!en_curso && goal_handle_actual_ &&
          goal_handle_actual_->is_active()) {
        auto result = std::make_shared<MoverBrazo::Result>();
        result->exito = true;
        result->mensaje = "Posición objetivo alcanzada con éxito.";
        goal_handle_actual_->succeed(result);
      }
    }

    // Publicación continua a RViz / Joint States.
    sensor_msgs::msg::JointState msg;
    msg.header.stamp = this->now();
    msg.name = {"Servomotor 1", "Servomotor 2", "Servomotor 3", "Servomotor 4"};
    msg.position = {
        servos_validos_.sm1.angulo_objetivo,
        servos_validos_.sm2.angulo_objetivo,
        servos_validos_.sm3.angulo_objetivo,
        servos_validos_.sm4.angulo_objetivo,
    };
    publisher_->publish(msg);
  }

  // Miembros de ROS 2
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr
      publisher_velocidades_;
  rclcpp_action::Server<MoverBrazo>::SharedPtr action_server_;
  std::shared_ptr<GoalHandleMoverBrazo> goal_handle_actual_;
  rclcpp::TimerBase::SharedPtr timer_;

  // Estado de movimiento y pose vigente (la planificación en sí vive
  // en Movimiento.h, agnóstica de ROS2)
  movimiento_t movimiento_{};
  servomotores_h servos_validos_;
  rclcpp::Time t_ref_;

  ik_continuidad_t continuidad_ik_{};

  PID_t pid_sm1_, pid_sm2_, pid_sm3_, pid_sm4_;
  bool control_cerrado_activo_ = false;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ControlBrazoNode>());
  rclcpp::shutdown();
  return 0;
}
