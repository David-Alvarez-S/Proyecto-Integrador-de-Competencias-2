// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from control_brazo:action/MoverBrazo.idl
// generated code does not contain a copyright notice
#include "control_brazo/action/detail/mover_brazo__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
control_brazo__action__MoverBrazo_Goal__init(control_brazo__action__MoverBrazo_Goal * msg)
{
  if (!msg) {
    return false;
  }
  // x
  // y
  // z
  return true;
}

void
control_brazo__action__MoverBrazo_Goal__fini(control_brazo__action__MoverBrazo_Goal * msg)
{
  if (!msg) {
    return;
  }
  // x
  // y
  // z
}

bool
control_brazo__action__MoverBrazo_Goal__are_equal(const control_brazo__action__MoverBrazo_Goal * lhs, const control_brazo__action__MoverBrazo_Goal * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // x
  if (lhs->x != rhs->x) {
    return false;
  }
  // y
  if (lhs->y != rhs->y) {
    return false;
  }
  // z
  if (lhs->z != rhs->z) {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_Goal__copy(
  const control_brazo__action__MoverBrazo_Goal * input,
  control_brazo__action__MoverBrazo_Goal * output)
{
  if (!input || !output) {
    return false;
  }
  // x
  output->x = input->x;
  // y
  output->y = input->y;
  // z
  output->z = input->z;
  return true;
}

control_brazo__action__MoverBrazo_Goal *
control_brazo__action__MoverBrazo_Goal__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Goal * msg = (control_brazo__action__MoverBrazo_Goal *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_Goal), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_Goal));
  bool success = control_brazo__action__MoverBrazo_Goal__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_Goal__destroy(control_brazo__action__MoverBrazo_Goal * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_Goal__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_Goal__Sequence__init(control_brazo__action__MoverBrazo_Goal__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Goal * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_Goal)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_Goal *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_Goal), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_Goal__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_Goal__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_Goal__Sequence__fini(control_brazo__action__MoverBrazo_Goal__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_Goal__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_Goal__Sequence *
control_brazo__action__MoverBrazo_Goal__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Goal__Sequence * array = (control_brazo__action__MoverBrazo_Goal__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_Goal__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_Goal__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_Goal__Sequence__destroy(control_brazo__action__MoverBrazo_Goal__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_Goal__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_Goal__Sequence__are_equal(const control_brazo__action__MoverBrazo_Goal__Sequence * lhs, const control_brazo__action__MoverBrazo_Goal__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_Goal__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_Goal__Sequence__copy(
  const control_brazo__action__MoverBrazo_Goal__Sequence * input,
  control_brazo__action__MoverBrazo_Goal__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_Goal)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_Goal);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_Goal * data =
      (control_brazo__action__MoverBrazo_Goal *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_Goal__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_Goal__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_Goal__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `mensaje`
#include "rosidl_runtime_c/string_functions.h"

bool
control_brazo__action__MoverBrazo_Result__init(control_brazo__action__MoverBrazo_Result * msg)
{
  if (!msg) {
    return false;
  }
  // exito
  // mensaje
  if (!rosidl_runtime_c__String__init(&msg->mensaje)) {
    control_brazo__action__MoverBrazo_Result__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_Result__fini(control_brazo__action__MoverBrazo_Result * msg)
{
  if (!msg) {
    return;
  }
  // exito
  // mensaje
  rosidl_runtime_c__String__fini(&msg->mensaje);
}

bool
control_brazo__action__MoverBrazo_Result__are_equal(const control_brazo__action__MoverBrazo_Result * lhs, const control_brazo__action__MoverBrazo_Result * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // exito
  if (lhs->exito != rhs->exito) {
    return false;
  }
  // mensaje
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->mensaje), &(rhs->mensaje)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_Result__copy(
  const control_brazo__action__MoverBrazo_Result * input,
  control_brazo__action__MoverBrazo_Result * output)
{
  if (!input || !output) {
    return false;
  }
  // exito
  output->exito = input->exito;
  // mensaje
  if (!rosidl_runtime_c__String__copy(
      &(input->mensaje), &(output->mensaje)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_Result *
control_brazo__action__MoverBrazo_Result__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Result * msg = (control_brazo__action__MoverBrazo_Result *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_Result), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_Result));
  bool success = control_brazo__action__MoverBrazo_Result__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_Result__destroy(control_brazo__action__MoverBrazo_Result * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_Result__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_Result__Sequence__init(control_brazo__action__MoverBrazo_Result__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Result * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_Result)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_Result *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_Result), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_Result__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_Result__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_Result__Sequence__fini(control_brazo__action__MoverBrazo_Result__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_Result__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_Result__Sequence *
control_brazo__action__MoverBrazo_Result__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Result__Sequence * array = (control_brazo__action__MoverBrazo_Result__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_Result__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_Result__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_Result__Sequence__destroy(control_brazo__action__MoverBrazo_Result__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_Result__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_Result__Sequence__are_equal(const control_brazo__action__MoverBrazo_Result__Sequence * lhs, const control_brazo__action__MoverBrazo_Result__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_Result__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_Result__Sequence__copy(
  const control_brazo__action__MoverBrazo_Result__Sequence * input,
  control_brazo__action__MoverBrazo_Result__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_Result)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_Result);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_Result * data =
      (control_brazo__action__MoverBrazo_Result *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_Result__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_Result__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_Result__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


bool
control_brazo__action__MoverBrazo_Feedback__init(control_brazo__action__MoverBrazo_Feedback * msg)
{
  if (!msg) {
    return false;
  }
  // progreso
  return true;
}

void
control_brazo__action__MoverBrazo_Feedback__fini(control_brazo__action__MoverBrazo_Feedback * msg)
{
  if (!msg) {
    return;
  }
  // progreso
}

bool
control_brazo__action__MoverBrazo_Feedback__are_equal(const control_brazo__action__MoverBrazo_Feedback * lhs, const control_brazo__action__MoverBrazo_Feedback * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // progreso
  if (lhs->progreso != rhs->progreso) {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_Feedback__copy(
  const control_brazo__action__MoverBrazo_Feedback * input,
  control_brazo__action__MoverBrazo_Feedback * output)
{
  if (!input || !output) {
    return false;
  }
  // progreso
  output->progreso = input->progreso;
  return true;
}

control_brazo__action__MoverBrazo_Feedback *
control_brazo__action__MoverBrazo_Feedback__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Feedback * msg = (control_brazo__action__MoverBrazo_Feedback *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_Feedback), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_Feedback));
  bool success = control_brazo__action__MoverBrazo_Feedback__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_Feedback__destroy(control_brazo__action__MoverBrazo_Feedback * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_Feedback__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_Feedback__Sequence__init(control_brazo__action__MoverBrazo_Feedback__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Feedback * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_Feedback)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_Feedback *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_Feedback), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_Feedback__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_Feedback__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_Feedback__Sequence__fini(control_brazo__action__MoverBrazo_Feedback__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_Feedback__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_Feedback__Sequence *
control_brazo__action__MoverBrazo_Feedback__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_Feedback__Sequence * array = (control_brazo__action__MoverBrazo_Feedback__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_Feedback__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_Feedback__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_Feedback__Sequence__destroy(control_brazo__action__MoverBrazo_Feedback__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_Feedback__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_Feedback__Sequence__are_equal(const control_brazo__action__MoverBrazo_Feedback__Sequence * lhs, const control_brazo__action__MoverBrazo_Feedback__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_Feedback__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_Feedback__Sequence__copy(
  const control_brazo__action__MoverBrazo_Feedback__Sequence * input,
  control_brazo__action__MoverBrazo_Feedback__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_Feedback)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_Feedback);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_Feedback * data =
      (control_brazo__action__MoverBrazo_Feedback *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_Feedback__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_Feedback__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_Feedback__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
#include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `goal`
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"

bool
control_brazo__action__MoverBrazo_SendGoal_Request__init(control_brazo__action__MoverBrazo_SendGoal_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    control_brazo__action__MoverBrazo_SendGoal_Request__fini(msg);
    return false;
  }
  // goal
  if (!control_brazo__action__MoverBrazo_Goal__init(&msg->goal)) {
    control_brazo__action__MoverBrazo_SendGoal_Request__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_SendGoal_Request__fini(control_brazo__action__MoverBrazo_SendGoal_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // goal
  control_brazo__action__MoverBrazo_Goal__fini(&msg->goal);
}

bool
control_brazo__action__MoverBrazo_SendGoal_Request__are_equal(const control_brazo__action__MoverBrazo_SendGoal_Request * lhs, const control_brazo__action__MoverBrazo_SendGoal_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // goal
  if (!control_brazo__action__MoverBrazo_Goal__are_equal(
      &(lhs->goal), &(rhs->goal)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_SendGoal_Request__copy(
  const control_brazo__action__MoverBrazo_SendGoal_Request * input,
  control_brazo__action__MoverBrazo_SendGoal_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // goal
  if (!control_brazo__action__MoverBrazo_Goal__copy(
      &(input->goal), &(output->goal)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_SendGoal_Request *
control_brazo__action__MoverBrazo_SendGoal_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Request * msg = (control_brazo__action__MoverBrazo_SendGoal_Request *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_SendGoal_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_SendGoal_Request));
  bool success = control_brazo__action__MoverBrazo_SendGoal_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_SendGoal_Request__destroy(control_brazo__action__MoverBrazo_SendGoal_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_SendGoal_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__init(control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_SendGoal_Request)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_SendGoal_Request *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_SendGoal_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_SendGoal_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_SendGoal_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__fini(control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_SendGoal_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_SendGoal_Request__Sequence *
control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * array = (control_brazo__action__MoverBrazo_SendGoal_Request__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_SendGoal_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__destroy(control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__are_equal(const control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * lhs, const control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_SendGoal_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__copy(
  const control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * input,
  control_brazo__action__MoverBrazo_SendGoal_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_SendGoal_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_SendGoal_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_SendGoal_Request * data =
      (control_brazo__action__MoverBrazo_SendGoal_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_SendGoal_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_SendGoal_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_SendGoal_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
control_brazo__action__MoverBrazo_SendGoal_Response__init(control_brazo__action__MoverBrazo_SendGoal_Response * msg)
{
  if (!msg) {
    return false;
  }
  // accepted
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    control_brazo__action__MoverBrazo_SendGoal_Response__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_SendGoal_Response__fini(control_brazo__action__MoverBrazo_SendGoal_Response * msg)
{
  if (!msg) {
    return;
  }
  // accepted
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
control_brazo__action__MoverBrazo_SendGoal_Response__are_equal(const control_brazo__action__MoverBrazo_SendGoal_Response * lhs, const control_brazo__action__MoverBrazo_SendGoal_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // accepted
  if (lhs->accepted != rhs->accepted) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_SendGoal_Response__copy(
  const control_brazo__action__MoverBrazo_SendGoal_Response * input,
  control_brazo__action__MoverBrazo_SendGoal_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // accepted
  output->accepted = input->accepted;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_SendGoal_Response *
control_brazo__action__MoverBrazo_SendGoal_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Response * msg = (control_brazo__action__MoverBrazo_SendGoal_Response *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_SendGoal_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_SendGoal_Response));
  bool success = control_brazo__action__MoverBrazo_SendGoal_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_SendGoal_Response__destroy(control_brazo__action__MoverBrazo_SendGoal_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_SendGoal_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__init(control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_SendGoal_Response)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_SendGoal_Response *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_SendGoal_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_SendGoal_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_SendGoal_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__fini(control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_SendGoal_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_SendGoal_Response__Sequence *
control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * array = (control_brazo__action__MoverBrazo_SendGoal_Response__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_SendGoal_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__destroy(control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__are_equal(const control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * lhs, const control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_SendGoal_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__copy(
  const control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * input,
  control_brazo__action__MoverBrazo_SendGoal_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_SendGoal_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_SendGoal_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_SendGoal_Response * data =
      (control_brazo__action__MoverBrazo_SendGoal_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_SendGoal_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_SendGoal_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_SendGoal_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
#include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"

bool
control_brazo__action__MoverBrazo_SendGoal_Event__init(control_brazo__action__MoverBrazo_SendGoal_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    control_brazo__action__MoverBrazo_SendGoal_Event__fini(msg);
    return false;
  }
  // request
  if (!control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__init(&msg->request, 0)) {
    control_brazo__action__MoverBrazo_SendGoal_Event__fini(msg);
    return false;
  }
  // response
  if (!control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__init(&msg->response, 0)) {
    control_brazo__action__MoverBrazo_SendGoal_Event__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_SendGoal_Event__fini(control_brazo__action__MoverBrazo_SendGoal_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__fini(&msg->request);
  // response
  control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__fini(&msg->response);
}

bool
control_brazo__action__MoverBrazo_SendGoal_Event__are_equal(const control_brazo__action__MoverBrazo_SendGoal_Event * lhs, const control_brazo__action__MoverBrazo_SendGoal_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_SendGoal_Event__copy(
  const control_brazo__action__MoverBrazo_SendGoal_Event * input,
  control_brazo__action__MoverBrazo_SendGoal_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_SendGoal_Event *
control_brazo__action__MoverBrazo_SendGoal_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Event * msg = (control_brazo__action__MoverBrazo_SendGoal_Event *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_SendGoal_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_SendGoal_Event));
  bool success = control_brazo__action__MoverBrazo_SendGoal_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_SendGoal_Event__destroy(control_brazo__action__MoverBrazo_SendGoal_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_SendGoal_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__init(control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_SendGoal_Event)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_SendGoal_Event *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_SendGoal_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_SendGoal_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_SendGoal_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__fini(control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_SendGoal_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_SendGoal_Event__Sequence *
control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * array = (control_brazo__action__MoverBrazo_SendGoal_Event__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_SendGoal_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__destroy(control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__are_equal(const control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * lhs, const control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_SendGoal_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_SendGoal_Event__Sequence__copy(
  const control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * input,
  control_brazo__action__MoverBrazo_SendGoal_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_SendGoal_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_SendGoal_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_SendGoal_Event * data =
      (control_brazo__action__MoverBrazo_SendGoal_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_SendGoal_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_SendGoal_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_SendGoal_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"

bool
control_brazo__action__MoverBrazo_GetResult_Request__init(control_brazo__action__MoverBrazo_GetResult_Request * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    control_brazo__action__MoverBrazo_GetResult_Request__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_GetResult_Request__fini(control_brazo__action__MoverBrazo_GetResult_Request * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
}

bool
control_brazo__action__MoverBrazo_GetResult_Request__are_equal(const control_brazo__action__MoverBrazo_GetResult_Request * lhs, const control_brazo__action__MoverBrazo_GetResult_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_GetResult_Request__copy(
  const control_brazo__action__MoverBrazo_GetResult_Request * input,
  control_brazo__action__MoverBrazo_GetResult_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_GetResult_Request *
control_brazo__action__MoverBrazo_GetResult_Request__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Request * msg = (control_brazo__action__MoverBrazo_GetResult_Request *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_GetResult_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_GetResult_Request));
  bool success = control_brazo__action__MoverBrazo_GetResult_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_GetResult_Request__destroy(control_brazo__action__MoverBrazo_GetResult_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_GetResult_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_GetResult_Request__Sequence__init(control_brazo__action__MoverBrazo_GetResult_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Request * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_GetResult_Request)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_GetResult_Request *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_GetResult_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_GetResult_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_GetResult_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_GetResult_Request__Sequence__fini(control_brazo__action__MoverBrazo_GetResult_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_GetResult_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_GetResult_Request__Sequence *
control_brazo__action__MoverBrazo_GetResult_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Request__Sequence * array = (control_brazo__action__MoverBrazo_GetResult_Request__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_GetResult_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_GetResult_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_GetResult_Request__Sequence__destroy(control_brazo__action__MoverBrazo_GetResult_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_GetResult_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_GetResult_Request__Sequence__are_equal(const control_brazo__action__MoverBrazo_GetResult_Request__Sequence * lhs, const control_brazo__action__MoverBrazo_GetResult_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_GetResult_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_GetResult_Request__Sequence__copy(
  const control_brazo__action__MoverBrazo_GetResult_Request__Sequence * input,
  control_brazo__action__MoverBrazo_GetResult_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_GetResult_Request)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_GetResult_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_GetResult_Request * data =
      (control_brazo__action__MoverBrazo_GetResult_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_GetResult_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_GetResult_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_GetResult_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `result`
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"

bool
control_brazo__action__MoverBrazo_GetResult_Response__init(control_brazo__action__MoverBrazo_GetResult_Response * msg)
{
  if (!msg) {
    return false;
  }
  // status
  // result
  if (!control_brazo__action__MoverBrazo_Result__init(&msg->result)) {
    control_brazo__action__MoverBrazo_GetResult_Response__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_GetResult_Response__fini(control_brazo__action__MoverBrazo_GetResult_Response * msg)
{
  if (!msg) {
    return;
  }
  // status
  // result
  control_brazo__action__MoverBrazo_Result__fini(&msg->result);
}

bool
control_brazo__action__MoverBrazo_GetResult_Response__are_equal(const control_brazo__action__MoverBrazo_GetResult_Response * lhs, const control_brazo__action__MoverBrazo_GetResult_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // status
  if (lhs->status != rhs->status) {
    return false;
  }
  // result
  if (!control_brazo__action__MoverBrazo_Result__are_equal(
      &(lhs->result), &(rhs->result)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_GetResult_Response__copy(
  const control_brazo__action__MoverBrazo_GetResult_Response * input,
  control_brazo__action__MoverBrazo_GetResult_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // status
  output->status = input->status;
  // result
  if (!control_brazo__action__MoverBrazo_Result__copy(
      &(input->result), &(output->result)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_GetResult_Response *
control_brazo__action__MoverBrazo_GetResult_Response__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Response * msg = (control_brazo__action__MoverBrazo_GetResult_Response *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_GetResult_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_GetResult_Response));
  bool success = control_brazo__action__MoverBrazo_GetResult_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_GetResult_Response__destroy(control_brazo__action__MoverBrazo_GetResult_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_GetResult_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_GetResult_Response__Sequence__init(control_brazo__action__MoverBrazo_GetResult_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Response * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_GetResult_Response)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_GetResult_Response *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_GetResult_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_GetResult_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_GetResult_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_GetResult_Response__Sequence__fini(control_brazo__action__MoverBrazo_GetResult_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_GetResult_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_GetResult_Response__Sequence *
control_brazo__action__MoverBrazo_GetResult_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Response__Sequence * array = (control_brazo__action__MoverBrazo_GetResult_Response__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_GetResult_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_GetResult_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_GetResult_Response__Sequence__destroy(control_brazo__action__MoverBrazo_GetResult_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_GetResult_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_GetResult_Response__Sequence__are_equal(const control_brazo__action__MoverBrazo_GetResult_Response__Sequence * lhs, const control_brazo__action__MoverBrazo_GetResult_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_GetResult_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_GetResult_Response__Sequence__copy(
  const control_brazo__action__MoverBrazo_GetResult_Response__Sequence * input,
  control_brazo__action__MoverBrazo_GetResult_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_GetResult_Response)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_GetResult_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_GetResult_Response * data =
      (control_brazo__action__MoverBrazo_GetResult_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_GetResult_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_GetResult_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_GetResult_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `info`
// already included above
// #include "service_msgs/msg/detail/service_event_info__functions.h"
// Member `request`
// Member `response`
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"

bool
control_brazo__action__MoverBrazo_GetResult_Event__init(control_brazo__action__MoverBrazo_GetResult_Event * msg)
{
  if (!msg) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__init(&msg->info)) {
    control_brazo__action__MoverBrazo_GetResult_Event__fini(msg);
    return false;
  }
  // request
  if (!control_brazo__action__MoverBrazo_GetResult_Request__Sequence__init(&msg->request, 0)) {
    control_brazo__action__MoverBrazo_GetResult_Event__fini(msg);
    return false;
  }
  // response
  if (!control_brazo__action__MoverBrazo_GetResult_Response__Sequence__init(&msg->response, 0)) {
    control_brazo__action__MoverBrazo_GetResult_Event__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_GetResult_Event__fini(control_brazo__action__MoverBrazo_GetResult_Event * msg)
{
  if (!msg) {
    return;
  }
  // info
  service_msgs__msg__ServiceEventInfo__fini(&msg->info);
  // request
  control_brazo__action__MoverBrazo_GetResult_Request__Sequence__fini(&msg->request);
  // response
  control_brazo__action__MoverBrazo_GetResult_Response__Sequence__fini(&msg->response);
}

bool
control_brazo__action__MoverBrazo_GetResult_Event__are_equal(const control_brazo__action__MoverBrazo_GetResult_Event * lhs, const control_brazo__action__MoverBrazo_GetResult_Event * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__are_equal(
      &(lhs->info), &(rhs->info)))
  {
    return false;
  }
  // request
  if (!control_brazo__action__MoverBrazo_GetResult_Request__Sequence__are_equal(
      &(lhs->request), &(rhs->request)))
  {
    return false;
  }
  // response
  if (!control_brazo__action__MoverBrazo_GetResult_Response__Sequence__are_equal(
      &(lhs->response), &(rhs->response)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_GetResult_Event__copy(
  const control_brazo__action__MoverBrazo_GetResult_Event * input,
  control_brazo__action__MoverBrazo_GetResult_Event * output)
{
  if (!input || !output) {
    return false;
  }
  // info
  if (!service_msgs__msg__ServiceEventInfo__copy(
      &(input->info), &(output->info)))
  {
    return false;
  }
  // request
  if (!control_brazo__action__MoverBrazo_GetResult_Request__Sequence__copy(
      &(input->request), &(output->request)))
  {
    return false;
  }
  // response
  if (!control_brazo__action__MoverBrazo_GetResult_Response__Sequence__copy(
      &(input->response), &(output->response)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_GetResult_Event *
control_brazo__action__MoverBrazo_GetResult_Event__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Event * msg = (control_brazo__action__MoverBrazo_GetResult_Event *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_GetResult_Event), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_GetResult_Event));
  bool success = control_brazo__action__MoverBrazo_GetResult_Event__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_GetResult_Event__destroy(control_brazo__action__MoverBrazo_GetResult_Event * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_GetResult_Event__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_GetResult_Event__Sequence__init(control_brazo__action__MoverBrazo_GetResult_Event__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Event * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_GetResult_Event)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_GetResult_Event *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_GetResult_Event), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_GetResult_Event__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_GetResult_Event__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_GetResult_Event__Sequence__fini(control_brazo__action__MoverBrazo_GetResult_Event__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_GetResult_Event__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_GetResult_Event__Sequence *
control_brazo__action__MoverBrazo_GetResult_Event__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_GetResult_Event__Sequence * array = (control_brazo__action__MoverBrazo_GetResult_Event__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_GetResult_Event__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_GetResult_Event__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_GetResult_Event__Sequence__destroy(control_brazo__action__MoverBrazo_GetResult_Event__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_GetResult_Event__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_GetResult_Event__Sequence__are_equal(const control_brazo__action__MoverBrazo_GetResult_Event__Sequence * lhs, const control_brazo__action__MoverBrazo_GetResult_Event__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_GetResult_Event__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_GetResult_Event__Sequence__copy(
  const control_brazo__action__MoverBrazo_GetResult_Event__Sequence * input,
  control_brazo__action__MoverBrazo_GetResult_Event__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_GetResult_Event)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_GetResult_Event);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_GetResult_Event * data =
      (control_brazo__action__MoverBrazo_GetResult_Event *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_GetResult_Event__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_GetResult_Event__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_GetResult_Event__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `goal_id`
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__functions.h"
// Member `feedback`
// already included above
// #include "control_brazo/action/detail/mover_brazo__functions.h"

bool
control_brazo__action__MoverBrazo_FeedbackMessage__init(control_brazo__action__MoverBrazo_FeedbackMessage * msg)
{
  if (!msg) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__init(&msg->goal_id)) {
    control_brazo__action__MoverBrazo_FeedbackMessage__fini(msg);
    return false;
  }
  // feedback
  if (!control_brazo__action__MoverBrazo_Feedback__init(&msg->feedback)) {
    control_brazo__action__MoverBrazo_FeedbackMessage__fini(msg);
    return false;
  }
  return true;
}

void
control_brazo__action__MoverBrazo_FeedbackMessage__fini(control_brazo__action__MoverBrazo_FeedbackMessage * msg)
{
  if (!msg) {
    return;
  }
  // goal_id
  unique_identifier_msgs__msg__UUID__fini(&msg->goal_id);
  // feedback
  control_brazo__action__MoverBrazo_Feedback__fini(&msg->feedback);
}

bool
control_brazo__action__MoverBrazo_FeedbackMessage__are_equal(const control_brazo__action__MoverBrazo_FeedbackMessage * lhs, const control_brazo__action__MoverBrazo_FeedbackMessage * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__are_equal(
      &(lhs->goal_id), &(rhs->goal_id)))
  {
    return false;
  }
  // feedback
  if (!control_brazo__action__MoverBrazo_Feedback__are_equal(
      &(lhs->feedback), &(rhs->feedback)))
  {
    return false;
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_FeedbackMessage__copy(
  const control_brazo__action__MoverBrazo_FeedbackMessage * input,
  control_brazo__action__MoverBrazo_FeedbackMessage * output)
{
  if (!input || !output) {
    return false;
  }
  // goal_id
  if (!unique_identifier_msgs__msg__UUID__copy(
      &(input->goal_id), &(output->goal_id)))
  {
    return false;
  }
  // feedback
  if (!control_brazo__action__MoverBrazo_Feedback__copy(
      &(input->feedback), &(output->feedback)))
  {
    return false;
  }
  return true;
}

control_brazo__action__MoverBrazo_FeedbackMessage *
control_brazo__action__MoverBrazo_FeedbackMessage__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_FeedbackMessage * msg = (control_brazo__action__MoverBrazo_FeedbackMessage *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_FeedbackMessage), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(control_brazo__action__MoverBrazo_FeedbackMessage));
  bool success = control_brazo__action__MoverBrazo_FeedbackMessage__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
control_brazo__action__MoverBrazo_FeedbackMessage__destroy(control_brazo__action__MoverBrazo_FeedbackMessage * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    control_brazo__action__MoverBrazo_FeedbackMessage__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__init(control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_FeedbackMessage * data = NULL;

  if (size) {
    if (size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_FeedbackMessage)) {
      return false;
    }
    data = (control_brazo__action__MoverBrazo_FeedbackMessage *)allocator.zero_allocate(size, sizeof(control_brazo__action__MoverBrazo_FeedbackMessage), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = control_brazo__action__MoverBrazo_FeedbackMessage__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        control_brazo__action__MoverBrazo_FeedbackMessage__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__fini(control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      control_brazo__action__MoverBrazo_FeedbackMessage__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

control_brazo__action__MoverBrazo_FeedbackMessage__Sequence *
control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * array = (control_brazo__action__MoverBrazo_FeedbackMessage__Sequence *)allocator.allocate(sizeof(control_brazo__action__MoverBrazo_FeedbackMessage__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__destroy(control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__are_equal(const control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * lhs, const control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!control_brazo__action__MoverBrazo_FeedbackMessage__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__copy(
  const control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * input,
  control_brazo__action__MoverBrazo_FeedbackMessage__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    if (input->size > SIZE_MAX / sizeof(control_brazo__action__MoverBrazo_FeedbackMessage)) {
      return false;
    }
    const size_t allocation_size =
      input->size * sizeof(control_brazo__action__MoverBrazo_FeedbackMessage);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    control_brazo__action__MoverBrazo_FeedbackMessage * data =
      (control_brazo__action__MoverBrazo_FeedbackMessage *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!control_brazo__action__MoverBrazo_FeedbackMessage__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          control_brazo__action__MoverBrazo_FeedbackMessage__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!control_brazo__action__MoverBrazo_FeedbackMessage__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
