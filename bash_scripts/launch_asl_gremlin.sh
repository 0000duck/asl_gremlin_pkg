#!/bin/bash

source this_robot_name.sh

# give the robot a name, so all the nodes will be launched under it's namespace
robot_name=$ROBOT_NAME

if [ "$robot_name" == "" ]; then
	robot_name="asl_gremlin1"
fi

# defining IP address to which ROS-MASTER should register or connect(if already MASTER is running with this IP)
IP=$(hostname -I | awk '{print $1}') # get the IP of the system

# defining port number of Arduino which controls motors and gets encoder ticks
arduino_port_num=/dev/ttyACM0

# ROS_MASTER_IP address
MASTER_IP=$IP
if [ "$#" == "1" ]; then

    default_IFS=$IFS
    IFS='.'
    master_ip_vec=($1)
    IFS=$default_IFS

    if [[ ${#master_ip_vec[@]} == 4 && ${master_ip_vec[0]} -le 255 &&  ${master_ip_vec[1]} -le 255\
          && ${master_ip_vec[2]} -le 255 && ${master_ip_vec[3]} -le 255 ]]; then
	    MASTER_IP=$1
    else
        echo "[ERROR] incorrect ROS_MASTER_IP provided $1, relaunch with correct IP"
        exit 1
    fi
fi

# Launch the nodes
export ROS_IP=$IP
export ROS_MASTER_URI=http://$MASTER_IP:11311

roslaunch asl_gremlin_pkg launch_asl_gremlin.launch robot_name:=$robot_name \
													arduino_port:=$arduino_port_num 
