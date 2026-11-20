import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    package_desc = 'descripcion_brazo'
    package_control = 'control_brazo'
    urdf_file_name = 'robot.urdf'

    pkg_share_desc = get_package_share_directory(package_desc)
    urdf_path = os.path.join(pkg_share_desc, 'urdf', urdf_file_name)

    with open(urdf_path, 'r') as infp:
        robot_desc = infp.read()

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_desc}]
    )

    nodo_mapeador_node = Node(
        package=package_control,
        executable='nodo_mapeador_torque',
        name='nodo_mapeador_torque',
        output='screen',
        parameters=[{'robot_description': robot_desc}]
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', os.path.join(pkg_share_desc, 'rviz', 'config_brazo.rviz')]
    )

    return LaunchDescription([
        robot_state_publisher_node,
        nodo_mapeador_node,
        rviz_node
    ])
