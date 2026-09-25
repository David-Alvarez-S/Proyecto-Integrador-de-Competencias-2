from setuptools import find_packages
from setuptools import setup

setup(
    name='control_brazo',
    version='0.0.0',
    packages=find_packages(
        include=('control_brazo', 'control_brazo.*')),
)
