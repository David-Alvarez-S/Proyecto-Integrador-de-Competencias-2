cmake_minimum_required(VERSION 3.8)
project(control_brazo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Buscar dependencias de ROS 2
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(kdl_parser REQUIRED)
find_package(orocos_kdl REQUIRED)
find_package(urdf REQUIRED)
find_package(ament_index_cpp REQUIRED)

# 2. Generar interfaces de ROS 2 (Action MoverBrazo)
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/MoverBrazo.action"
  DEPENDENCIES std_msgs
)

# 3. Incluir directorios de cabeceras (.h)
include_directories(
  include
  include/${PROJECT_NAME}
)

# 4. EJECUTABLE 1: Nodo Mapeador de Torque (Barrido Grid + CSV + RViz Markers)
add_executable(nodo_mapeador_torque src/nodo_mapeador_torque.cpp)
ament_target_dependencies(nodo_mapeador_torque
  rclcpp
  sensor_msgs
  std_msgs
  visualization_msgs
  kdl_parser
  orocos_kdl
  urdf
  ament_index_cpp
)

install(TARGETS 
  nodo_mapeador_torque
  DESTINATION lib/${PROJECT_NAME}
)

# 5. EJECUTABLE 2: Nodo Control de Brazo (Action Server para trayectorias)
add_executable(nodo_control_brazo src/nodo_control.cpp)
ament_target_dependencies(nodo_control_brazo
  rclcpp
  rclcpp_action
  sensor_msgs
  std_msgs
)

# Enlazar la acción MoverBrazo generada al nodo de control
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(nodo_control_brazo 
  "${cpp_typesupport_target}"
)

# Instalación explícita de carpetas de recursos
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
  OPTIONAL
)

install(DIRECTORY urdf/
  DESTINATION share/${PROJECT_NAME}/urdf
  OPTIONAL
)

install(DIRECTORY assets/
  DESTINATION share/${PROJECT_NAME}/assets 
  OPTIONAL
)

install(FILES robot.urdf
  DESTINATION share/${PROJECT_NAME}/urdf
  OPTIONAL
)

install(TARGETS 
  nodo_mapeador_torque
  nodo_control_brazo
  DESTINATION lib/${PROJECT_NAME}
)


ament_package()
