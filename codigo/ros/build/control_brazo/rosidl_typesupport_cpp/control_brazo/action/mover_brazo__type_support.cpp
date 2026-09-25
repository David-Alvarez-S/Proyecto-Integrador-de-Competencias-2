// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from control_brazo:action/MoverBrazo.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "control_brazo/action/detail/mover_brazo__functions.h"
#include "control_brazo/action/detail/mover_brazo__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_Goal_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_Goal_type_support_ids_t;

static const _MoverBrazo_Goal_type_support_ids_t _MoverBrazo_Goal_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_Goal_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_Goal_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_Goal_type_support_symbol_names_t _MoverBrazo_Goal_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_Goal)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_Goal)),
  }
};

typedef struct _MoverBrazo_Goal_type_support_data_t
{
  void * data[2];
} _MoverBrazo_Goal_type_support_data_t;

static _MoverBrazo_Goal_type_support_data_t _MoverBrazo_Goal_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_Goal_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_Goal_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_Goal_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_Goal_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_Goal_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_Goal_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_Goal__get_type_hash,
  &control_brazo__action__MoverBrazo_Goal__get_type_description,
  &control_brazo__action__MoverBrazo_Goal__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_Goal>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_Goal_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_Goal)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_Goal>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_Result_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_Result_type_support_ids_t;

static const _MoverBrazo_Result_type_support_ids_t _MoverBrazo_Result_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_Result_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_Result_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_Result_type_support_symbol_names_t _MoverBrazo_Result_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_Result)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_Result)),
  }
};

typedef struct _MoverBrazo_Result_type_support_data_t
{
  void * data[2];
} _MoverBrazo_Result_type_support_data_t;

static _MoverBrazo_Result_type_support_data_t _MoverBrazo_Result_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_Result_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_Result_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_Result_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_Result_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_Result_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_Result_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_Result__get_type_hash,
  &control_brazo__action__MoverBrazo_Result__get_type_description,
  &control_brazo__action__MoverBrazo_Result__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_Result>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_Result_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_Result)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_Result>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_Feedback_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_Feedback_type_support_ids_t;

static const _MoverBrazo_Feedback_type_support_ids_t _MoverBrazo_Feedback_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_Feedback_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_Feedback_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_Feedback_type_support_symbol_names_t _MoverBrazo_Feedback_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_Feedback)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_Feedback)),
  }
};

typedef struct _MoverBrazo_Feedback_type_support_data_t
{
  void * data[2];
} _MoverBrazo_Feedback_type_support_data_t;

static _MoverBrazo_Feedback_type_support_data_t _MoverBrazo_Feedback_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_Feedback_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_Feedback_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_Feedback_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_Feedback_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_Feedback_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_Feedback_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_Feedback__get_type_hash,
  &control_brazo__action__MoverBrazo_Feedback__get_type_description,
  &control_brazo__action__MoverBrazo_Feedback__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_Feedback>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_Feedback_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_Feedback)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_Feedback>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_SendGoal_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_Request_type_support_ids_t;

static const _MoverBrazo_SendGoal_Request_type_support_ids_t _MoverBrazo_SendGoal_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_SendGoal_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_SendGoal_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_SendGoal_Request_type_support_symbol_names_t _MoverBrazo_SendGoal_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_SendGoal_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_SendGoal_Request)),
  }
};

typedef struct _MoverBrazo_SendGoal_Request_type_support_data_t
{
  void * data[2];
} _MoverBrazo_SendGoal_Request_type_support_data_t;

static _MoverBrazo_SendGoal_Request_type_support_data_t _MoverBrazo_SendGoal_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_SendGoal_Request_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_SendGoal_Request_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_SendGoal_Request_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_SendGoal_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_SendGoal_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_SendGoal_Request__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal_Request__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Request>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_SendGoal_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_SendGoal_Request)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_SendGoal_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_Response_type_support_ids_t;

static const _MoverBrazo_SendGoal_Response_type_support_ids_t _MoverBrazo_SendGoal_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_SendGoal_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_SendGoal_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_SendGoal_Response_type_support_symbol_names_t _MoverBrazo_SendGoal_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_SendGoal_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_SendGoal_Response)),
  }
};

typedef struct _MoverBrazo_SendGoal_Response_type_support_data_t
{
  void * data[2];
} _MoverBrazo_SendGoal_Response_type_support_data_t;

static _MoverBrazo_SendGoal_Response_type_support_data_t _MoverBrazo_SendGoal_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_SendGoal_Response_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_SendGoal_Response_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_SendGoal_Response_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_SendGoal_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_SendGoal_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_SendGoal_Response__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal_Response__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Response>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_SendGoal_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_SendGoal_Response)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_SendGoal_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_Event_type_support_ids_t;

static const _MoverBrazo_SendGoal_Event_type_support_ids_t _MoverBrazo_SendGoal_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_SendGoal_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_SendGoal_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_SendGoal_Event_type_support_symbol_names_t _MoverBrazo_SendGoal_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_SendGoal_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_SendGoal_Event)),
  }
};

typedef struct _MoverBrazo_SendGoal_Event_type_support_data_t
{
  void * data[2];
} _MoverBrazo_SendGoal_Event_type_support_data_t;

static _MoverBrazo_SendGoal_Event_type_support_data_t _MoverBrazo_SendGoal_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_SendGoal_Event_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_SendGoal_Event_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_SendGoal_Event_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_SendGoal_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_SendGoal_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_SendGoal_Event__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal_Event__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Event>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_SendGoal_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_SendGoal_Event)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_SendGoal_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_type_support_ids_t;

static const _MoverBrazo_SendGoal_type_support_ids_t _MoverBrazo_SendGoal_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_SendGoal_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_SendGoal_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_SendGoal_type_support_symbol_names_t _MoverBrazo_SendGoal_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_SendGoal)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_SendGoal)),
  }
};

typedef struct _MoverBrazo_SendGoal_type_support_data_t
{
  void * data[2];
} _MoverBrazo_SendGoal_type_support_data_t;

static _MoverBrazo_SendGoal_type_support_data_t _MoverBrazo_SendGoal_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_SendGoal_service_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_SendGoal_service_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_SendGoal_service_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_SendGoal_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t MoverBrazo_SendGoal_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<control_brazo::action::MoverBrazo_SendGoal_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<control_brazo::action::MoverBrazo_SendGoal>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<control_brazo::action::MoverBrazo_SendGoal>,
  &control_brazo__action__MoverBrazo_SendGoal__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<control_brazo::action::MoverBrazo_SendGoal>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_SendGoal_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_SendGoal)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<control_brazo::action::MoverBrazo_SendGoal>();
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_GetResult_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_Request_type_support_ids_t;

static const _MoverBrazo_GetResult_Request_type_support_ids_t _MoverBrazo_GetResult_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_GetResult_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_GetResult_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_GetResult_Request_type_support_symbol_names_t _MoverBrazo_GetResult_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_GetResult_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_GetResult_Request)),
  }
};

typedef struct _MoverBrazo_GetResult_Request_type_support_data_t
{
  void * data[2];
} _MoverBrazo_GetResult_Request_type_support_data_t;

static _MoverBrazo_GetResult_Request_type_support_data_t _MoverBrazo_GetResult_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_GetResult_Request_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_GetResult_Request_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_GetResult_Request_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_GetResult_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_GetResult_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_GetResult_Request__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult_Request__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Request>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_GetResult_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_GetResult_Request)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_GetResult_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_Response_type_support_ids_t;

static const _MoverBrazo_GetResult_Response_type_support_ids_t _MoverBrazo_GetResult_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_GetResult_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_GetResult_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_GetResult_Response_type_support_symbol_names_t _MoverBrazo_GetResult_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_GetResult_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_GetResult_Response)),
  }
};

typedef struct _MoverBrazo_GetResult_Response_type_support_data_t
{
  void * data[2];
} _MoverBrazo_GetResult_Response_type_support_data_t;

static _MoverBrazo_GetResult_Response_type_support_data_t _MoverBrazo_GetResult_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_GetResult_Response_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_GetResult_Response_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_GetResult_Response_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_GetResult_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_GetResult_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_GetResult_Response__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult_Response__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Response>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_GetResult_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_GetResult_Response)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_GetResult_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_Event_type_support_ids_t;

static const _MoverBrazo_GetResult_Event_type_support_ids_t _MoverBrazo_GetResult_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_GetResult_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_GetResult_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_GetResult_Event_type_support_symbol_names_t _MoverBrazo_GetResult_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_GetResult_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_GetResult_Event)),
  }
};

typedef struct _MoverBrazo_GetResult_Event_type_support_data_t
{
  void * data[2];
} _MoverBrazo_GetResult_Event_type_support_data_t;

static _MoverBrazo_GetResult_Event_type_support_data_t _MoverBrazo_GetResult_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_GetResult_Event_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_GetResult_Event_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_GetResult_Event_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_GetResult_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_GetResult_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_GetResult_Event__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult_Event__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Event>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_GetResult_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_GetResult_Event)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_GetResult_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_type_support_ids_t;

static const _MoverBrazo_GetResult_type_support_ids_t _MoverBrazo_GetResult_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_GetResult_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_GetResult_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_GetResult_type_support_symbol_names_t _MoverBrazo_GetResult_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_GetResult)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_GetResult)),
  }
};

typedef struct _MoverBrazo_GetResult_type_support_data_t
{
  void * data[2];
} _MoverBrazo_GetResult_type_support_data_t;

static _MoverBrazo_GetResult_type_support_data_t _MoverBrazo_GetResult_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_GetResult_service_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_GetResult_service_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_GetResult_service_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_GetResult_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t MoverBrazo_GetResult_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<control_brazo::action::MoverBrazo_GetResult_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<control_brazo::action::MoverBrazo_GetResult>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<control_brazo::action::MoverBrazo_GetResult>,
  &control_brazo__action__MoverBrazo_GetResult__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<control_brazo::action::MoverBrazo_GetResult>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_GetResult_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_GetResult)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<control_brazo::action::MoverBrazo_GetResult>();
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _MoverBrazo_FeedbackMessage_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_FeedbackMessage_type_support_ids_t;

static const _MoverBrazo_FeedbackMessage_type_support_ids_t _MoverBrazo_FeedbackMessage_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _MoverBrazo_FeedbackMessage_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _MoverBrazo_FeedbackMessage_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _MoverBrazo_FeedbackMessage_type_support_symbol_names_t _MoverBrazo_FeedbackMessage_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, control_brazo, action, MoverBrazo_FeedbackMessage)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, control_brazo, action, MoverBrazo_FeedbackMessage)),
  }
};

typedef struct _MoverBrazo_FeedbackMessage_type_support_data_t
{
  void * data[2];
} _MoverBrazo_FeedbackMessage_type_support_data_t;

static _MoverBrazo_FeedbackMessage_type_support_data_t _MoverBrazo_FeedbackMessage_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _MoverBrazo_FeedbackMessage_message_typesupport_map = {
  2,
  "control_brazo",
  &_MoverBrazo_FeedbackMessage_message_typesupport_ids.typesupport_identifier[0],
  &_MoverBrazo_FeedbackMessage_message_typesupport_symbol_names.symbol_name[0],
  &_MoverBrazo_FeedbackMessage_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t MoverBrazo_FeedbackMessage_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_FeedbackMessage_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_FeedbackMessage__get_type_hash,
  &control_brazo__action__MoverBrazo_FeedbackMessage__get_type_description,
  &control_brazo__action__MoverBrazo_FeedbackMessage__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<control_brazo::action::MoverBrazo_FeedbackMessage>()
{
  return &::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_FeedbackMessage_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo_FeedbackMessage)() {
  return get_message_type_support_handle<control_brazo::action::MoverBrazo_FeedbackMessage>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

#include "action_msgs/msg/goal_status_array.hpp"
#include "action_msgs/srv/cancel_goal.hpp"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_typesupport_cpp/action_type_support.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_cpp/service_type_support.hpp"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_cpp
{

static rosidl_action_type_support_t MoverBrazo_action_type_support_handle = {
  NULL, NULL, NULL, NULL, NULL,
  &control_brazo__action__MoverBrazo__get_type_hash,
  &control_brazo__action__MoverBrazo__get_type_description,
  &control_brazo__action__MoverBrazo__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace control_brazo

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_action_type_support_t *
get_action_type_support_handle<control_brazo::action::MoverBrazo>()
{
  using ::control_brazo::action::rosidl_typesupport_cpp::MoverBrazo_action_type_support_handle;
  // Thread-safe by always writing the same values to the static struct
  MoverBrazo_action_type_support_handle.goal_service_type_support = get_service_type_support_handle<::control_brazo::action::MoverBrazo::Impl::SendGoalService>();
  MoverBrazo_action_type_support_handle.result_service_type_support = get_service_type_support_handle<::control_brazo::action::MoverBrazo::Impl::GetResultService>();
  MoverBrazo_action_type_support_handle.cancel_service_type_support = get_service_type_support_handle<::control_brazo::action::MoverBrazo::Impl::CancelGoalService>();
  MoverBrazo_action_type_support_handle.feedback_message_type_support = get_message_type_support_handle<::control_brazo::action::MoverBrazo::Impl::FeedbackMessage>();
  MoverBrazo_action_type_support_handle.status_message_type_support = get_message_type_support_handle<::control_brazo::action::MoverBrazo::Impl::GoalStatusMessage>();
  return &MoverBrazo_action_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_action_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(rosidl_typesupport_cpp, control_brazo, action, MoverBrazo)() {
  return ::rosidl_typesupport_cpp::get_action_type_support_handle<control_brazo::action::MoverBrazo>();
}

#ifdef __cplusplus
}
#endif
