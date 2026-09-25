#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

#include "control_brazo/action/mover_brazo.hpp"

extern "C" {
#include "control_brazo/Aritmetica.h"
#include "control_brazo/Cuaterniones.h"
#include "control_brazo/Global_Config.h"
#include "control_brazo/IK.h"
#include "control_brazo/Interpolaciones.h"
#include "control_brazo/PID.h"
#include "control_brazo/Perfil_S.h"
#include "control_brazo/Vectores.h"
}

using namespace std::chrono_literals;

class ControlBrazoNode : public rclcpp::Node {
public:
  using MoverBrazo = control_brazo::action::MoverBrazo;

  using GoalHandleMoverBrazo = rclcpp_action::ServerGoalHandle<MoverBrazo>;

  ControlBrazoNode() : Node("nodo_control_brazo"), en_movimiento_(false) {
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

    // Posición inicial por defecto al encender
    v_actual_ = {250.0f, 0.0f, 50.0f};
    v_inicio_ = v_actual_;
    v_fin_ = v_actual_;

    limites_perfil_s_t limites = {V_MAX, A_MAX, J_MAX};
    Iniciar_Perfil_S(&planificador_, limites);

    // Bucle de control periódico a 50Hz (20ms)
    timer_ = this->create_wall_timer(
        20ms, std::bind(&ControlBrazoNode::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "Action Server 'mover_brazo' listo.");
  }

private:
  // --- CALLBACKS DEL ACTION SERVER ---

  // Se ejecuta cuando un cliente envía un objetivo
  rclcpp_action::GoalResponse
  handle_goal(const rclcpp_action::GoalUUID &uuid,
              std::shared_ptr<const MoverBrazo::Goal> goal) {
    (void)uuid;
    RCLCPP_INFO(this->get_logger(),
                "Nueva meta recibida: X=%.2f, Y=%.2f, Z=%.2f", goal->x, goal->y,
                goal->z);
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

    v_inicio_ = v_actual_;
    v_fin_ = {goal->x, goal->y, goal->z};

    // Replanificar el perfil S desde s=0 hasta s=1
    Planificar_Perfil_s(&planificador_, 0.0f, 1.0f);
    t_ref_ = this->now();
    en_movimiento_ = true;
  }

  // --- BUCLE DE CONTROL DEL ROBOT ---
  void timer_callback() {
    if (en_movimiento_) {
      const float t = static_cast<float>((this->now() - t_ref_).seconds());
      estado_perfil_s_t estado = Estado_Perfil_s(&planificador_, t);
      const float s = estado.posicion;

      // Actualizar posición cartesiana actual mediante LERP
      lerp(&v_actual_, v_inicio_, v_fin_, s);

      // Enviar Feedback al cliente que solicitó la acción
      if (goal_handle_actual_) {
        auto feedback = std::make_shared<MoverBrazo::Feedback>();
        feedback->progreso = s;
        goal_handle_actual_->publish_feedback(feedback);
      }

      // Comprobar si la trayectoria terminó
      if (estado.termino) {
        en_movimiento_ = false;

        if (goal_handle_actual_ && goal_handle_actual_->is_active()) {
          auto result = std::make_shared<MoverBrazo::Result>();
          result->exito = true;
          result->mensaje = "Posición objetivo alcanzada con éxito.";
          goal_handle_actual_->succeed(result);
        }
      }
    }

    // Cinemática Inversa con la pose cartesiana v_actual_
    servomotores_h servos = servos_validos_;
    if (IK(&servos, v_actual_)) {
      servos_validos_ = servos;
    } else {
      RCLCPP_WARN_THROTTLE(
          get_logger(), *get_clock(), 1000,
          "IK fuera de alcance, manteniendo última pose válida");
    }
    // Publicación continua a RViz / Joint States
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
  rclcpp_action::Server<MoverBrazo>::SharedPtr action_server_;
  std::shared_ptr<GoalHandleMoverBrazo> goal_handle_actual_;
  rclcpp::TimerBase::SharedPtr timer_;

  // Variables de cinematica y perfiles
  planificador_perfil_s_t planificador_;
  vector_t v_actual_;
  vector_t v_inicio_;
  vector_t v_fin_;
  servomotores_h servos_validos_;
  rclcpp::Time t_ref_;
  bool en_movimiento_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ControlBrazoNode>());
  rclcpp::shutdown();
  return 0;
}
