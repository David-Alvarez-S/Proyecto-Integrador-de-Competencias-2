// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from control_brazo:action/MoverBrazo.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "control_brazo/action/mover_brazo.hpp"


#ifndef CONTROL_BRAZO__ACTION__DETAIL__MOVER_BRAZO__BUILDER_HPP_
#define CONTROL_BRAZO__ACTION__DETAIL__MOVER_BRAZO__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "control_brazo/action/detail/mover_brazo__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_Goal_z
{
public:
  explicit Init_MoverBrazo_Goal_z(::control_brazo::action::MoverBrazo_Goal & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_Goal z(::control_brazo::action::MoverBrazo_Goal::_z_type arg)
  {
    msg_.z = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_Goal msg_;
};

class Init_MoverBrazo_Goal_y
{
public:
  explicit Init_MoverBrazo_Goal_y(::control_brazo::action::MoverBrazo_Goal & msg)
  : msg_(msg)
  {}
  Init_MoverBrazo_Goal_z y(::control_brazo::action::MoverBrazo_Goal::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_MoverBrazo_Goal_z(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_Goal msg_;
};

class Init_MoverBrazo_Goal_x
{
public:
  Init_MoverBrazo_Goal_x()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_Goal_y x(::control_brazo::action::MoverBrazo_Goal::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_MoverBrazo_Goal_y(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_Goal msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_Goal>()
{
  return control_brazo::action::builder::Init_MoverBrazo_Goal_x();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_Result_mensaje
{
public:
  explicit Init_MoverBrazo_Result_mensaje(::control_brazo::action::MoverBrazo_Result & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_Result mensaje(::control_brazo::action::MoverBrazo_Result::_mensaje_type arg)
  {
    msg_.mensaje = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_Result msg_;
};

class Init_MoverBrazo_Result_exito
{
public:
  Init_MoverBrazo_Result_exito()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_Result_mensaje exito(::control_brazo::action::MoverBrazo_Result::_exito_type arg)
  {
    msg_.exito = std::move(arg);
    return Init_MoverBrazo_Result_mensaje(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_Result msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_Result>()
{
  return control_brazo::action::builder::Init_MoverBrazo_Result_exito();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_Feedback_progreso
{
public:
  Init_MoverBrazo_Feedback_progreso()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::control_brazo::action::MoverBrazo_Feedback progreso(::control_brazo::action::MoverBrazo_Feedback::_progreso_type arg)
  {
    msg_.progreso = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_Feedback msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_Feedback>()
{
  return control_brazo::action::builder::Init_MoverBrazo_Feedback_progreso();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_SendGoal_Request_goal
{
public:
  explicit Init_MoverBrazo_SendGoal_Request_goal(::control_brazo::action::MoverBrazo_SendGoal_Request & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_SendGoal_Request goal(::control_brazo::action::MoverBrazo_SendGoal_Request::_goal_type arg)
  {
    msg_.goal = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Request msg_;
};

class Init_MoverBrazo_SendGoal_Request_goal_id
{
public:
  Init_MoverBrazo_SendGoal_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_SendGoal_Request_goal goal_id(::control_brazo::action::MoverBrazo_SendGoal_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_MoverBrazo_SendGoal_Request_goal(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_SendGoal_Request>()
{
  return control_brazo::action::builder::Init_MoverBrazo_SendGoal_Request_goal_id();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_SendGoal_Response_stamp
{
public:
  explicit Init_MoverBrazo_SendGoal_Response_stamp(::control_brazo::action::MoverBrazo_SendGoal_Response & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_SendGoal_Response stamp(::control_brazo::action::MoverBrazo_SendGoal_Response::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Response msg_;
};

class Init_MoverBrazo_SendGoal_Response_accepted
{
public:
  Init_MoverBrazo_SendGoal_Response_accepted()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_SendGoal_Response_stamp accepted(::control_brazo::action::MoverBrazo_SendGoal_Response::_accepted_type arg)
  {
    msg_.accepted = std::move(arg);
    return Init_MoverBrazo_SendGoal_Response_stamp(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_SendGoal_Response>()
{
  return control_brazo::action::builder::Init_MoverBrazo_SendGoal_Response_accepted();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_SendGoal_Event_response
{
public:
  explicit Init_MoverBrazo_SendGoal_Event_response(::control_brazo::action::MoverBrazo_SendGoal_Event & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_SendGoal_Event response(::control_brazo::action::MoverBrazo_SendGoal_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Event msg_;
};

class Init_MoverBrazo_SendGoal_Event_request
{
public:
  explicit Init_MoverBrazo_SendGoal_Event_request(::control_brazo::action::MoverBrazo_SendGoal_Event & msg)
  : msg_(msg)
  {}
  Init_MoverBrazo_SendGoal_Event_response request(::control_brazo::action::MoverBrazo_SendGoal_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_MoverBrazo_SendGoal_Event_response(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Event msg_;
};

class Init_MoverBrazo_SendGoal_Event_info
{
public:
  Init_MoverBrazo_SendGoal_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_SendGoal_Event_request info(::control_brazo::action::MoverBrazo_SendGoal_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_MoverBrazo_SendGoal_Event_request(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_SendGoal_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_SendGoal_Event>()
{
  return control_brazo::action::builder::Init_MoverBrazo_SendGoal_Event_info();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_GetResult_Request_goal_id
{
public:
  Init_MoverBrazo_GetResult_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::control_brazo::action::MoverBrazo_GetResult_Request goal_id(::control_brazo::action::MoverBrazo_GetResult_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_GetResult_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_GetResult_Request>()
{
  return control_brazo::action::builder::Init_MoverBrazo_GetResult_Request_goal_id();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_GetResult_Response_result
{
public:
  explicit Init_MoverBrazo_GetResult_Response_result(::control_brazo::action::MoverBrazo_GetResult_Response & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_GetResult_Response result(::control_brazo::action::MoverBrazo_GetResult_Response::_result_type arg)
  {
    msg_.result = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_GetResult_Response msg_;
};

class Init_MoverBrazo_GetResult_Response_status
{
public:
  Init_MoverBrazo_GetResult_Response_status()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_GetResult_Response_result status(::control_brazo::action::MoverBrazo_GetResult_Response::_status_type arg)
  {
    msg_.status = std::move(arg);
    return Init_MoverBrazo_GetResult_Response_result(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_GetResult_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_GetResult_Response>()
{
  return control_brazo::action::builder::Init_MoverBrazo_GetResult_Response_status();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_GetResult_Event_response
{
public:
  explicit Init_MoverBrazo_GetResult_Event_response(::control_brazo::action::MoverBrazo_GetResult_Event & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_GetResult_Event response(::control_brazo::action::MoverBrazo_GetResult_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_GetResult_Event msg_;
};

class Init_MoverBrazo_GetResult_Event_request
{
public:
  explicit Init_MoverBrazo_GetResult_Event_request(::control_brazo::action::MoverBrazo_GetResult_Event & msg)
  : msg_(msg)
  {}
  Init_MoverBrazo_GetResult_Event_response request(::control_brazo::action::MoverBrazo_GetResult_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_MoverBrazo_GetResult_Event_response(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_GetResult_Event msg_;
};

class Init_MoverBrazo_GetResult_Event_info
{
public:
  Init_MoverBrazo_GetResult_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_GetResult_Event_request info(::control_brazo::action::MoverBrazo_GetResult_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_MoverBrazo_GetResult_Event_request(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_GetResult_Event msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_GetResult_Event>()
{
  return control_brazo::action::builder::Init_MoverBrazo_GetResult_Event_info();
}

}  // namespace control_brazo


namespace control_brazo
{

namespace action
{

namespace builder
{

class Init_MoverBrazo_FeedbackMessage_feedback
{
public:
  explicit Init_MoverBrazo_FeedbackMessage_feedback(::control_brazo::action::MoverBrazo_FeedbackMessage & msg)
  : msg_(msg)
  {}
  ::control_brazo::action::MoverBrazo_FeedbackMessage feedback(::control_brazo::action::MoverBrazo_FeedbackMessage::_feedback_type arg)
  {
    msg_.feedback = std::move(arg);
    return std::move(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_FeedbackMessage msg_;
};

class Init_MoverBrazo_FeedbackMessage_goal_id
{
public:
  Init_MoverBrazo_FeedbackMessage_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_MoverBrazo_FeedbackMessage_feedback goal_id(::control_brazo::action::MoverBrazo_FeedbackMessage::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_MoverBrazo_FeedbackMessage_feedback(msg_);
  }

private:
  ::control_brazo::action::MoverBrazo_FeedbackMessage msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::control_brazo::action::MoverBrazo_FeedbackMessage>()
{
  return control_brazo::action::builder::Init_MoverBrazo_FeedbackMessage_goal_id();
}

}  // namespace control_brazo

#endif  // CONTROL_BRAZO__ACTION__DETAIL__MOVER_BRAZO__BUILDER_HPP_
