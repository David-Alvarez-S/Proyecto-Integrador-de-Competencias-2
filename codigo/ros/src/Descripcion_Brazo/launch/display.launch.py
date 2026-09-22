import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    # 1. Configuración de rutas y nombres
    package_name = 'descripcion_brazo'  
    urdf_file_name = 'Brazo.urdf'       

    # Obtener las rutas de instalación
    pkg_share = get_package_share_directory(package_name)
    urdf_path = os.path.join(pkg_share, 'urdf', urdf_file_name)

    # Leer el contenido del URDF (para pasarlo como parámetro)
    with open(urdf_path, 'r') as infp:
        robot_desc = infp.read()

    # 2. Definir los nodos a ejecutar

    # Nodo obligatorio: Publica el estado del robot en base al URDF
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[{'robot_description': robot_desc}]
    )

    # Nodo de GUI: Abre la ventana con sliders para mover articulaciones
    joint_state_publisher_gui_node = Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
        name='joint_state_publisher_gui'
    )

    # Nodo de visualización: Abre RViz 2
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(pkg_share, 'rviz', 'display.rviz')], # Si tienes config, sino abre RViz por defecto
        condition=None # Abre RViz siempre
    )
    
    # Si no tienes un archivo de configuración de RViz, puedes usar esta versión más simple de rviz_node:
    # rviz_node = Node(
    #     package='rviz2',
    #     executable='rviz2',
    #     name='rviz2'
    # )

    # 3. Retornar la descripción del lanzamiento
    return LaunchDescription([
        robot_state_publisher_node,
        joint_state_publisher_gui_node,
        rviz_node
    ])
