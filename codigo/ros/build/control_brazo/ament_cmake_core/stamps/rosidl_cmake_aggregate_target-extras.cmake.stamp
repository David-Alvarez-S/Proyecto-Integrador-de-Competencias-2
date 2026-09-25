# generated from rosidl_cmake/cmake/rosidl_cmake_aggregate_target-extras.cmake.in

# Create a convenience aggregate target control_brazo::control_brazo
# that links all generated interface targets, so downstream packages can use
# a single modern CMake target name instead of ${control_brazo_TARGETS}.
if(control_brazo_TARGETS AND NOT TARGET control_brazo::control_brazo)
  add_library(control_brazo::control_brazo INTERFACE IMPORTED)
  set_target_properties(control_brazo::control_brazo PROPERTIES
    INTERFACE_LINK_LIBRARIES "${control_brazo_TARGETS}")
endif()
