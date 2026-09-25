// generated from rosidl_typesupport_c/resource/idl__type_support.cpp.em
// with input from control_brazo:action/MoverBrazo.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "control_brazo/action/detail/mover_brazo__struct.h"
#include "control_brazo/action/detail/mover_brazo__type_support.h"
#include "control_brazo/action/detail/mover_brazo__functions.h"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/message_type_support_dispatch.h"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_c/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_Goal_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_Goal_type_support_ids_t;

static const _MoverBrazo_Goal_type_support_ids_t _MoverBrazo_Goal_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_Goal)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_Goal)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_Goal_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_Goal__get_type_hash,
  &control_brazo__action__MoverBrazo_Goal__get_type_description,
  &control_brazo__action__MoverBrazo_Goal__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_Goal)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_Goal_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_Result_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_Result_type_support_ids_t;

static const _MoverBrazo_Result_type_support_ids_t _MoverBrazo_Result_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_Result)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_Result)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_Result_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_Result__get_type_hash,
  &control_brazo__action__MoverBrazo_Result__get_type_description,
  &control_brazo__action__MoverBrazo_Result__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_Result)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_Result_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_Feedback_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_Feedback_type_support_ids_t;

static const _MoverBrazo_Feedback_type_support_ids_t _MoverBrazo_Feedback_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_Feedback)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_Feedback)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_Feedback_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_Feedback__get_type_hash,
  &control_brazo__action__MoverBrazo_Feedback__get_type_description,
  &control_brazo__action__MoverBrazo_Feedback__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_Feedback)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_Feedback_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_SendGoal_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_Request_type_support_ids_t;

static const _MoverBrazo_SendGoal_Request_type_support_ids_t _MoverBrazo_SendGoal_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_SendGoal_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_SendGoal_Request)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_Request_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_SendGoal_Request__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal_Request__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_SendGoal_Request)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_SendGoal_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_SendGoal_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_Response_type_support_ids_t;

static const _MoverBrazo_SendGoal_Response_type_support_ids_t _MoverBrazo_SendGoal_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_SendGoal_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_SendGoal_Response)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_Response_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_SendGoal_Response__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal_Response__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_SendGoal_Response)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_SendGoal_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_SendGoal_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_Event_type_support_ids_t;

static const _MoverBrazo_SendGoal_Event_type_support_ids_t _MoverBrazo_SendGoal_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_SendGoal_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_SendGoal_Event)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_Event_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_SendGoal_Event__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal_Event__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_SendGoal_Event)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_SendGoal_Event_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/service_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
#include "service_msgs/msg/service_event_info.h"
#include "builtin_interfaces/msg/time.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{
typedef struct _MoverBrazo_SendGoal_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_SendGoal_type_support_ids_t;

static const _MoverBrazo_SendGoal_type_support_ids_t _MoverBrazo_SendGoal_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_SendGoal)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_SendGoal)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_SendGoal_service_typesupport_map),
  rosidl_typesupport_c__get_service_typesupport_handle_function,
  &MoverBrazo_SendGoal_Request_message_type_support_handle,
  &MoverBrazo_SendGoal_Response_message_type_support_handle,
  &MoverBrazo_SendGoal_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    control_brazo,
    action,
    MoverBrazo_SendGoal
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    control_brazo,
    action,
    MoverBrazo_SendGoal
  ),
  &control_brazo__action__MoverBrazo_SendGoal__get_type_hash,
  &control_brazo__action__MoverBrazo_SendGoal__get_type_description,
  &control_brazo__action__MoverBrazo_SendGoal__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_SendGoal)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_SendGoal_service_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_GetResult_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_Request_type_support_ids_t;

static const _MoverBrazo_GetResult_Request_type_support_ids_t _MoverBrazo_GetResult_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_GetResult_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_GetResult_Request)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_Request_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_GetResult_Request__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult_Request__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_GetResult_Request)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_GetResult_Request_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_GetResult_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_Response_type_support_ids_t;

static const _MoverBrazo_GetResult_Response_type_support_ids_t _MoverBrazo_GetResult_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_GetResult_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_GetResult_Response)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_Response_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_GetResult_Response__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult_Response__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_GetResult_Response)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_GetResult_Response_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_GetResult_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_Event_type_support_ids_t;

static const _MoverBrazo_GetResult_Event_type_support_ids_t _MoverBrazo_GetResult_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_GetResult_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_GetResult_Event)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_Event_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_GetResult_Event__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult_Event__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_GetResult_Event)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_GetResult_Event_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/service_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"
// already included above
// #include "service_msgs/msg/service_event_info.h"
// already included above
// #include "builtin_interfaces/msg/time.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{
typedef struct _MoverBrazo_GetResult_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_GetResult_type_support_ids_t;

static const _MoverBrazo_GetResult_type_support_ids_t _MoverBrazo_GetResult_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_GetResult)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_GetResult)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_GetResult_service_typesupport_map),
  rosidl_typesupport_c__get_service_typesupport_handle_function,
  &MoverBrazo_GetResult_Request_message_type_support_handle,
  &MoverBrazo_GetResult_Response_message_type_support_handle,
  &MoverBrazo_GetResult_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    control_brazo,
    action,
    MoverBrazo_GetResult
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    control_brazo,
    action,
    MoverBrazo_GetResult
  ),
  &control_brazo__action__MoverBrazo_GetResult__get_type_hash,
  &control_brazo__action__MoverBrazo_GetResult__get_type_description,
  &control_brazo__action__MoverBrazo_GetResult__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_GetResult)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_GetResult_service_type_support_handle;
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__struct.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"
// already included above
// #include "rosidl_typesupport_c/identifier.h"
// already included above
// #include "rosidl_typesupport_c/message_type_support_dispatch.h"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_c/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace control_brazo
{

namespace action
{

namespace rosidl_typesupport_c
{

typedef struct _MoverBrazo_FeedbackMessage_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _MoverBrazo_FeedbackMessage_type_support_ids_t;

static const _MoverBrazo_FeedbackMessage_type_support_ids_t _MoverBrazo_FeedbackMessage_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_c",  // ::rosidl_typesupport_fastrtps_c::typesupport_identifier,
    "rosidl_typesupport_introspection_c",  // ::rosidl_typesupport_introspection_c::typesupport_identifier,
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
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, control_brazo, action, MoverBrazo_FeedbackMessage)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, control_brazo, action, MoverBrazo_FeedbackMessage)),
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
  rosidl_typesupport_c__typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_MoverBrazo_FeedbackMessage_message_typesupport_map),
  rosidl_typesupport_c__get_message_typesupport_handle_function,
  &control_brazo__action__MoverBrazo_FeedbackMessage__get_type_hash,
  &control_brazo__action__MoverBrazo_FeedbackMessage__get_type_description,
  &control_brazo__action__MoverBrazo_FeedbackMessage__get_type_description_sources,
};

}  // namespace rosidl_typesupport_c

}  // namespace action

}  // namespace control_brazo

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_c, control_brazo, action, MoverBrazo_FeedbackMessage)() {
  return &::control_brazo::action::rosidl_typesupport_c::MoverBrazo_FeedbackMessage_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif

#include "action_msgs/msg/goal_status_array.h"
#include "action_msgs/srv/cancel_goal.h"
#include "control_brazo/action/mover_brazo.h"
// already included above
// #include "control_brazo/action/detail/mover_brazo__type_support.h"

static rosidl_action_type_support_t _control_brazo__action__MoverBrazo__typesupport_c = {
  NULL, NULL, NULL, NULL, NULL,
  &control_brazo__action__MoverBrazo__get_type_hash,
  &control_brazo__action__MoverBrazo__get_type_description,
  &control_brazo__action__MoverBrazo__get_type_description_sources,
};

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_action_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(
  rosidl_typesupport_c, control_brazo, action, MoverBrazo)()
{
  // Thread-safe by always writing the same values to the static struct
  _control_brazo__action__MoverBrazo__typesupport_c.goal_service_type_support =
    ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(
    rosidl_typesupport_c, control_brazo, action, MoverBrazo_SendGoal)();
  _control_brazo__action__MoverBrazo__typesupport_c.result_service_type_support =
    ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(
    rosidl_typesupport_c, control_brazo, action, MoverBrazo_GetResult)();
  _control_brazo__action__MoverBrazo__typesupport_c.cancel_service_type_support =
    ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(
    rosidl_typesupport_c, action_msgs, srv, CancelGoal)();
  _control_brazo__action__MoverBrazo__typesupport_c.feedback_message_type_support =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c, control_brazo, action, MoverBrazo_FeedbackMessage)();
  _control_brazo__action__MoverBrazo__typesupport_c.status_message_type_support =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c, action_msgs, msg, GoalStatusArray)();

  return &_control_brazo__action__MoverBrazo__typesupport_c;
}

#ifdef __cplusplus
}
#endif
