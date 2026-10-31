#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float32_multi_array.hpp"

#include "control_brazo/action/mover_brazo.hpp"

extern "C" {
#include "control_brazo/Aritmetica.h"
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

    // Pose inicial de arranque (Home)
    v_actual_ = {300.0f, 0.0f, 50.0f};
    if (!IK_ext(&servos_validos_, v_actual_, 0.0f, 0.0f, &continuidad_ik_,
                nullptr)) {
      RCLCPP_ERROR(this->get_logger(),
                   "Pose inicial inalcanzable: revisar Global_Config.h.");
    }

    // Inicialización de PIDs para encoders AS5600
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
    RCLCPP_INFO(this->get_logger(), "Evaluando meta: X=%.2f, Y=%.2f, Z=%.2f",
                goal->x, goal->y, goal->z);

    vector_t v_meta = {goal->x, goal->y, goal->z};

    if (!Movimiento_Es_Alcanzable(v_meta)) {
      RCLCPP_WARN(
          this->get_logger(),
          "RECHAZADO: El vector (%.2f, %.2f, %.2f) no paso IK_es_alcanzable.",
          goal->x, goal->y, goal->z);
      return rclcpp_action::GoalResponse::REJECT;
    }

    RCLCPP_INFO(this->get_logger(), "META ACEPTADA");
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  rclcpp_action::CancelResponse
  handle_cancel(const std::shared_ptr<GoalHandleMoverBrazo> goal_handle) {
    (void)goal_handle;
    RCLCPP_INFO(this->get_logger(), "Cancelación de movimiento solicitada.");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  void
  handle_accepted(const std::shared_ptr<GoalHandleMoverBrazo> goal_handle) {
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

    // Pasa v_actual_ e v_meta correctamente
    if (!Movimiento_Iniciar(&movimiento_, servos_validos_, v_actual_, v_meta,
                            limites, &continuidad_ik_)) {

      RCLCPP_ERROR(this->get_logger(),
                   "IK falló al iniciar movimiento hacia (%.2f, %.2f, %.2f). "
                   "Meta abortada.",
                   v_meta.x, v_meta.y, v_meta.z);
      auto result = std::make_shared<MoverBrazo::Result>();
      result->exito = false;
      result->mensaje = "Error interno de cinemática inversa.";
      goal_handle->abort(result);
      goal_handle_actual_.reset();
      return;
    }

    v_meta_actual_ = v_meta;
    t_ref_ = this->now();
  }

  void timer_callback() {
    if (movimiento_.activo) {
      const float t = static_cast<float>((this->now() - t_ref_).seconds());

      float progreso = 0.0f;
      // Evaluación ciclo a ciclo con actualización de continuidad
      bool en_curso = Movimiento_Evaluar(&servos_validos_, &movimiento_, t,
                                         &continuidad_ik_, &progreso);

      if (goal_handle_actual_) {
        auto feedback = std::make_shared<MoverBrazo::Feedback>();
        feedback->progreso = progreso;
        goal_handle_actual_->publish_feedback(feedback);
      }

      if (!en_curso && goal_handle_actual_ &&
          goal_handle_actual_->is_active()) {

        // Movimiento completado: actualizar la posición cartesiana de partida
        v_actual_ = v_meta_actual_;

        auto result = std::make_shared<MoverBrazo::Result>();
        result->exito = true;
        result->mensaje = "Posición objetivo alcanzada con éxito.";
        goal_handle_actual_->succeed(result);
      }
    }

    // Publicación periódica en /joint_states
    sensor_msgs::msg::JointState msg;
    msg.header.stamp = this->now();
    msg.name = {"revolute_1", "revolute_2", "revolute_3", "revolute_4"};
    msg.position = {
        servos_validos_.sm1.angulo_objetivo,
        servos_validos_.sm2.angulo_objetivo,
        servos_validos_.sm3.angulo_objetivo,
        servos_validos_.sm4.angulo_objetivo,
    };
    publisher_->publish(msg);
  }

  // Miembros de ROS 2 y estado local
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr
      publisher_velocidades_;
  rclcpp_action::Server<MoverBrazo>::SharedPtr action_server_;
  std::shared_ptr<GoalHandleMoverBrazo> goal_handle_actual_;
  rclcpp::TimerBase::SharedPtr timer_;

  movimiento_t movimiento_{};
  servomotores_h servos_validos_;
  vector_t v_actual_{};
  vector_t v_meta_actual_{};
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
