#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <state_feedback/EncoderDataToOmega.h>
#include <asl_gremlin_msgs/MotorAngVel.h>
#include <asl_gremlin_pkg/GetParam.h>
#include <asl_gremlin_pkg/SubscribeTopic.h>

#include <cmath>
#include <string>

using namespace state_feedback;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "encoder_data_to_omega");
    ROS_INFO("Initialized:= %s",ros::this_node::getName().c_str());

    ros::NodeHandle enco2w_nh;

    std::string actual_w_topic;
    actual_w_topic = asl_gremlin_pkg::GetParam_with_shutdown<std::string>
                        (enco2w_nh, "state_feedback/encoder/ang_vel_topic", __LINE__);

    ros::Publisher enco2w_pub = enco2w_nh.advertise<asl_gremlin_msgs::MotorAngVel>(actual_w_topic, 100);

    asl_gremlin_pkg::SubscribeTopic < std_msgs::Bool > sim(enco2w_nh, "start_sim");

    EncoderDataToOmega encoder_data_to_omega(enco2w_nh);

    asl_gremlin_msgs::MotorAngVel motor_ang_vel;
    motor_ang_vel.header.frame_id = "none";

    double rate = 10.0;
    if (!enco2w_nh.getParam("sim/rate", rate))
    {
        ROS_WARN("Unable access parameter /%s/sim/rate, setting rate as 10Hz",
                  ros::this_node::getNamespace().c_str());
    }
    ros::Rate loop_rate(rate);

    int msg_count = 0;
    bool initiated = false;


    while(ros::ok())
    {
        if ( !initiated && (sim.get_data())->data )
        {
            encoder_data_to_omega.update_encoder_starting_values();
            initiated = true;
        }
        if ( !(sim.get_data())->data && initiated)
        {
            initiated = false;
        }
        encoder_data_to_omega.calculate_angular_velocities();
        motor_ang_vel.wl = encoder_data_to_omega.get_left_wheel_angular_vel();
        motor_ang_vel.wr = encoder_data_to_omega.get_right_wheel_angular_vel();
        motor_ang_vel.header.seq = msg_count;
        motor_ang_vel.header.stamp = ros::Time::now();
        
        enco2w_pub.publish(motor_ang_vel);
        ros::spinOnce();
        loop_rate.sleep();
    }
}
