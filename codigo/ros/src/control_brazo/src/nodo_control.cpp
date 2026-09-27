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

// Período del lazo de control (s). Se usa para crear el timer.
#define CONTROL_DT_S 0.02f

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

    // Pose inicial por defecto al encender. Se resuelve UNA sola vez
    // acá (no en cada tick): el resto del tiempo el estado "real" del
    // brazo son los ángulos en servos_validos_, no una posición
    // cartesiana.
    vector_t v_inicial = {250.0f, 0.0f, 50.0f};
    if (!IK_ext(&servos_validos_, v_inicial, 0.0f, 0.0f, nullptr)) {
      RCLCPP_ERROR(this->get_logger(),
                   "Pose inicial inalcanzable: revisar Global_Config.h.");
    }
    servos_inicio_ = servos_validos_;
    servos_fin_ = servos_validos_;

    limites_perfil_s_t limites = {V_MAX, A_MAX, J_MAX};
    Iniciar_Perfil_S(&planificador_, limites);

    // Bucle de control periódico (20ms == CONTROL_DT_S)
    timer_ = this->create_wall_timer(
        std::chrono::duration<float>(CONTROL_DT_S),
        std::bind(&ControlBrazoNode::timer_callback, this));

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

    // Rechazo temprano: si el punto final ya es geométricamente
    // inalcanzable, ni siquiera se acepta el goal. Chequeo sin estado,
    // seguro de llamar aunque haya un movimiento en curso.
    vector_t v_meta = {goal->x, goal->y, goal->z};
    if (!IK_es_alcanzable(v_meta)) {
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

    // El punto de partida en espacio articular es exactamente la
    // última pose comandada: garantiza continuidad exacta con el
    // movimiento anterior (incluso si venía interrumpido a mitad de
    // camino), sin volver a llamar a IK para el punto de inicio.
    servos_inicio_ = servos_validos_;

    // IK del punto final. error_max=0 y dt=0 porque handle_goal ya
    // confirmó con IK_es_alcanzable que entra en el espacio de
    // trabajo: no debería hacer falta tolerancia. Se chequea el
    // resultado de todos modos, por las dudas (defensivo).
    vector_t v_meta = {goal->x, goal->y, goal->z};
    servomotores_h servos_meta;
    if (!IK_ext(&servos_meta, v_meta, 0.0f, 0.0f, nullptr)) {
      RCLCPP_ERROR(
          this->get_logger(),
          "IK falló pese a validación previa (bug o L_* inconsistentes "
          "entre IK_es_alcanzable e IK_ext). Meta abortada.");
      auto result = std::make_shared<MoverBrazo::Result>();
      result->exito = false;
      result->mensaje = "Error interno de cinemática inversa.";
      goal_handle->abort(result);
      goal_handle_actual_.reset();
      return;
    }
    servos_fin_ = servos_meta;

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

      // Interpolación en espacio de articulaciones: cada ángulo es una
      // función AFÍN de s(t), así que hereda directamente vel/acc/jerk
      // acotados del perfil S en los cuatro servos, sin excepción.
      // No hay atan2/acos en el medio del camino (esos solo se
      // evaluaron una vez, al fijar servos_inicio_/servos_fin_), así
      // que no hay forma de que esto "escape" del perfil ni de que
      // necesite tolerancia o límite de velocidad de base.
      servos_validos_.sm1.angulo_objetivo =
          lerp_angulo(servos_inicio_.sm1.angulo_objetivo,
                      servos_fin_.sm1.angulo_objetivo, s);
      servos_validos_.sm2.angulo_objetivo =
          lerp_angulo(servos_inicio_.sm2.angulo_objetivo,
                      servos_fin_.sm2.angulo_objetivo, s);
      servos_validos_.sm3.angulo_objetivo =
          lerp_angulo(servos_inicio_.sm3.angulo_objetivo,
                      servos_fin_.sm3.angulo_objetivo, s);
      servos_validos_.sm4.angulo_objetivo =
          lerp_angulo(servos_inicio_.sm4.angulo_objetivo,
                      servos_fin_.sm4.angulo_objetivo, s);

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

    // Publicación continua a RViz / Joint States. servos_validos_ ya
    // tiene los ángulos resueltos arriba (o la última pose válida si
    // no hay movimiento en curso) — no hace falta llamar a IK acá.
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

  // Variables de cinematica y perfiles (ahora en espacio articular)
  planificador_perfil_s_t planificador_;
  servomotores_h servos_inicio_;
  servomotores_h servos_fin_;
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
