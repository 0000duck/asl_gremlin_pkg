#!/bin/bash

# give the robot a name, so all the nodes will be launched under it's namespace
robot_name=asl_gremlin1 

# defining IP address to which ROS-MASTER should register or connect(if already MASTER is running with this IP)
IP=$(hostname -I) # get the IP of the system
IP=$(echo "${IP}" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//') # remove leading and trailing spaces

# defining port number of Arduino which controls motors and gets encoder ticks
encoder_motor_port=/dev/ttyACM0

# Launch the nodes
export ROS_IP=$IP
export ROS_MASTER_URI=http://$IP:11311

roslaunch asl_gremlin_pkg launch_asl_gremlin.launch robot_name:=$robot_name \
													encoder_port:=$encoder_motor_port
