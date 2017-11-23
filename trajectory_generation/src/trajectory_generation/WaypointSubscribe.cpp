/**
 * @brief WaypointSubscribe definitions
 * @file WaypointSubscribe.cpp
 * @author Murali VNV <muralivnv@gmail.com>
 */
/*
 * Copyright (c) 2017, muralivnv
 *
 * This file is part of the asl_gremlin_package and subject to the license terms
 * in the top-level LICENSE file of the asl_gremlin_pkg repository.
 * https://github.com/muralivnv/asl_gremlin_pkg/blob/master/LICENSE
 */
#include <trajectory_generation/WaypointSubscribe.h>

WaypointSubscribe::WaypointSubscribe(ros::NodeHandle& nh)
{
    fun_ = boost::bind(&WaypointSubscribe::dynamic_reconfigure_waypointSet_callback,this,
                       _1, _2);

    dr_wp_srv_.setCallback(fun_);
    x_waypoints_ = {0};
    y_waypoints_ = {0};

    sim_stop_pub_ = nh.advertise<std_msgs::Bool>("start_sim",10);
}

void WaypointSubscribe::dynamic_reconfigure_waypointSet_callback(trajectory_generation::waypointSetConfig & config,  uint32_t level)
{
    x_waypoints_ = utility_pkg::string_to_vector<double>(config.X_waypoint);
    y_waypoints_ = utility_pkg::string_to_vector<double>(config.Y_waypoint);

    if (  x_waypoints_.size() != y_waypoints_.size() )
    {
        ROS_ERROR(" trajectory_generation: x and y waypoints are not of equal size, change them");
        
        x_waypoints_.erase(std::begin(x_waypoints_), std::end(x_waypoints_));
        y_waypoints_.erase(std::begin(y_waypoints_), std::end(y_waypoints_));
    }
    else if ( x_waypoints_.size() == 1 && x_waypoints_[0] == 0 && y_waypoints_[0] == 0 )
    { 
        ROS_WARN(" trajectory_generation: waypoint stack is empty, use 'rosservice' to set them "); 
       
        x_waypoints_.erase(std::begin(x_waypoints_), std::end(x_waypoints_));
        y_waypoints_.erase(std::begin(y_waypoints_), std::end(y_waypoints_));
    }
    else
    {
        received_wp_ = true;
        ROS_INFO(" trajectory_generation: waypoints received ...");
       
        if (std::fabs(x_waypoints_[0]) < 2 && std::fabs(y_waypoints_[0]) < 2)
        { 
            x_waypoints_.erase(std::begin(x_waypoints_));
            y_waypoints_.erase(std::begin(y_waypoints_));
        }

        std::cout << "x_waypoints: ";
        utility_pkg::print_stl_container(x_waypoints_);
        
        std::cout << "\ny_waypoints: ";
        utility_pkg::print_stl_container(y_waypoints_);
    }
}

std::vector<double> WaypointSubscribe::get_current_waypoint()
{
    return {x_waypoints_[current_waypoint_ptr_],
                y_waypoints_[current_waypoint_ptr_] };
}


std::vector<double> WaypointSubscribe::get_next_waypoint()
{
    ++current_waypoint_ptr_;
    if (current_waypoint_ptr_ == x_waypoints_.size())
    {
        sim_stop_pub_.publish(std_msgs::Bool());
        --current_waypoint_ptr_;
        return {0};
    }
    return {x_waypoints_[current_waypoint_ptr_],
                y_waypoints_[current_waypoint_ptr_] };
}

void WaypointSubscribe::reset_counter()
{ current_waypoint_ptr_ = 0; }

