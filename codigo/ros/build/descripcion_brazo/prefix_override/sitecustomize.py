import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/agustin/Proyectos/Proyecto-Integrador-de-Competencias-2/codigo/ros/install/descripcion_brazo'
