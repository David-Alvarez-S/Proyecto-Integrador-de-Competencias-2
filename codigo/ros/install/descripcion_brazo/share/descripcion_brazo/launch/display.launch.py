import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    package_name = 'descripcion_brazo'  
    urdf_file_name = 'Brazo.urdf'       

    pkg_share = get_package_share_directory(package_name)
    urdf_path = os.path.join(pkg_share, 'urdf', urdf_file_name)

    with open(urdf_path, 'r') as infp:
        robot_desc = infp.read()

    # 1. Publicador del estado del robot
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[{'robot_description': robot_desc}]
    )

    # 2. Tu nodo de control C++ traído desde el otro paquete ('control_brazo')
    nodo_control_node = Node(
        package='control_brazo',
        executable='nodo_control',
        name='nodo_control'
    )

    # 3. RViz2
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', os.path.join(pkg_share, 'rviz', 'config_brazo.rviz')]
    )
    
    return LaunchDescription([
        robot_state_publisher_node,
        nodo_control_node,
        rviz_node
    ])
