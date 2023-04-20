#!/usr/bin/env python3
import rospy
from math import sin, cos, pi
from typing import List
from geometry_msgs.msg import Quaternion, Twist
from nav_msgs.msg import Odometry
from diagnostic_msgs.msg import *
from tf.broadcaster import TransformBroadcaster
from ax12 import AX12
from controllers import Controller
from struct import unpack


class DiffController(Controller):
    """Controller to handle movement & odometry feedback for a differential
    drive mobile base."""

    def __init__(self, device: AX12, name: str) -> None:
        super().__init__(device, name)
        self.pause = True
        self.last_cmd = rospy.Time.now()

        # Load parameters
        self._load_params(name)

        # Initialize internal data
        self._initialize_internal_data()

        # Setup ROS publishers, subscribers and broadcasters
        self._setup_ros_communication(name)

        rospy.loginfo(f"Started DiffController ({name}). Geometry: {self.base_width:.2f} m wide, {self.ticks_meter:.2f} ticks/m.")

    def _load_params(self, name: str) -> None:
        """Load ROS parameters."""
        # Rates and geometry
        self.rate = rospy.get_param('~controllers/'+name+'/rate', 10.0)
        self.timeout = rospy.get_param('~controllers/'+name+'/timeout', 1.0)
        self.t_delta = rospy.Duration(1.0/self.rate)
        self.t_next = rospy.Time.now() + self.t_delta
        self.ticks_meter = float(rospy.get_param('~controllers/'+name+'/ticks_meter'))
        self.base_width = float(rospy.get_param('~controllers/'+name+'/base_width'))

        # Frame IDs
        self.base_frame_id = rospy.get_param('~controllers/'+name+'/base_frame_id', 'base_link')
        self.odom_frame_id = rospy.get_param('~controllers/'+name+'/odom_frame_id', 'odom')

        # PID parameters
        self.Kp = rospy.get_param('~controllers/'+name+'/Kp', 5)
        self.Kd = rospy.get_param('~controllers/'+name+'/Kd', 1)
        self.Ki = rospy.get_param('~controllers/'+name+'/Ki', 0)
        self.Ko = rospy.get_param('~controllers/'+name+'/Ko', 50)

        # Acceleration parameters
        self.accel_limit = rospy.get_param('~controllers/'+name+'/accel_limit', 0.1)
        self.max_accel = int(self.accel_limit*self.ticks_meter/self.rate)

    def _initialize_internal_data(self) -> None:
        """Initialize internal data variables."""
        self.v_left = 0                 # current setpoint velocity
        self.v_right = 0
        self.v_des_left = 0             # cmd_vel setpoint
        self.v_des_right = 0
        self.enc_left = None            # encoder readings
        self.enc_right = None
        self.x = 0                      # position in xy plane
        self.y = 0
        self.th = 0
        self.dx = 0                     # speeds in x/rotation
        self.dr = 0
        self.then = rospy.Time.now()    # time for determining dx/dy

    def _setup_ros_communication(self, name: str) -> None:
        """Setup ROS publishers, subscribers, and broadcasters."""
        rospy.Subscriber("forklift_drive_controller/cmd_vel", Twist, self.cmd_vel_cb)
        self.odom_pub = rospy.Publisher("odom", Odometry, queue_size=
