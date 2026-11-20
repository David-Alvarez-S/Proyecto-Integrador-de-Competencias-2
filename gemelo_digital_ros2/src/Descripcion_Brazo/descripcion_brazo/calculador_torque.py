import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import Float64MultiArray
import csv
import PyKDL
from kdl_parser_py.urdf import treeFromString

class CalculadorTorqueEstatico(Node):
    def __init__(self):
        super().__init__('calculador_torque_estatico')
        self.declare_parameter('robot_description', '')
        robot_desc = self.get_parameter('robot_description').value
        
        if not robot_desc:
            self.get_logger().error("FALTA PARÁMETRO: robot_description está vacío.")
            return

        exito, arbol = treeFromString(robot_desc)
        if not exito:
            self.get_logger().error("Fallo al parsear el URDF con KDL.")
            return
        self.cadena = arbol.getChain("base", "acople_4")
        
        gravedad = PyKDL.Vector(0.0, 0.0, -9.81)
        self.dyn_solver = PyKDL.ChainDynParam(self.cadena, gravedad)
        
        self.num_joints = self.cadena.getNrOfJoints()
        self.posiciones = PyKDL.JntArray(self.num_joints)
        self.torques = PyKDL.JntArray(self.num_joints)

        self.create_subscription(JointState, '/joint_states', self.joint_callback, 10)
        self.publisher_torques = self.create_publisher(Float64MultiArray, '/joint_torques', 10)
        
        self.csv_path = 'torques_estaticos.csv'
        self.csv_file = open(self.csv_path, 'w', newline='')
        self.writer = csv.writer(self.csv_file)
        self.writer.writerow(['timestamp', 'joint_1_Nm', 'joint_2_Nm', 'joint_3_Nm', 'joint_4_Nm'])
        
        self.get_logger().info('Nodo dinámico KDL iniciado. Calculando con pesos del URDF.')

    def joint_callback(self, msg):
        if len(msg.position) < self.num_joints:
            return
            
        for i in range(self.num_joints):
            self.posiciones[i] = msg.position[i]
            
        self.dyn_solver.JntToGravity(self.posiciones, self.torques)
        
        torques_absolutos = []
        for i in range(self.num_joints):
            torques_absolutos.append(abs(self.torques[i]))

        out_msg = Float64MultiArray()
        out_msg.data = torques_absolutos
        self.publisher_torques.publish(out_msg)
        
        t_stamp = msg.header.stamp.sec + (msg.header.stamp.nanosec * 1e-9)
        self.writer.writerow([t_stamp] + torques_absolutos)

    def destroy_node(self):
        self.csv_file.close()
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = CalculadorTorqueEstatico()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
