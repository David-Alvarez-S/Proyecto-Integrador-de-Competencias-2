import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'descripcion_brazo'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    
        # Instalar el archivo URDF en share/descripcion_brazo/urdf/
        (os.path.join('share', package_name, 'urdf'), glob('urdf/*.urdf') if glob('urdf/*.urdf') else ['robot.urdf']),
    
        # Instalar los archivos .stl de assets en share/descripcion_brazo/assets/
        (os.path.join('share', package_name, 'assets'), glob('assets/*.stl')),
    
        # Archivos de launch y rviz
        (os.path.join('share', package_name, 'launch'), glob('launch/*')),
        (os.path.join('share', package_name, 'rviz'), glob('rviz/*')),
    ], 
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='agustin',
    maintainer_email='agustin@todo.todo',
    description='Paquete URDF para el brazo robotico',
    license='Apache-2.0',
    extras_require={
        'test': ['pytest'],
    },
    entry_points={
        'console_scripts': [
            'calculador_torque = descripcion_brazo.calculador_torque:main',
        ],
    },
)
