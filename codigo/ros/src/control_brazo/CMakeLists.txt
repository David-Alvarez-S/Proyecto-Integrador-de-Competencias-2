cmake_minimum_required(VERSION 3.8)
project(control_brazo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED) 
find_package(sensor_msgs REQUIRED)
find_package(interactive_markers REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Generar archivos C++ a partir del .action
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/MoverBrazo.action"
)

add_executable(nodo_control src/nodo_control.cpp)

target_include_directories(nodo_control PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(nodo_control 
  rclcpp 
  rclcpp_action 
  sensor_msgs 
  interactive_markers
)

# AÑADIDO: Enlazar la librería generada de la acción (MoverBrazo.action)
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(nodo_control "${cpp_typesupport_target}" m)

install(TARGETS nodo_control
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
