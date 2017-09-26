#include <controller/OmegaToPWM.h>

using namespace controller;
using namespace utility_pkg::custom_algorithms;

OmegaToPWM::OmegaToPWM(ros::NodeHandle& nh)
{
    if (!nh.getParam("/asl_gremlin/motor/omega_to_pwm/omega", omega_lookup_))
    {
        utility_pkg::throw_error_and_shutdown("/asl_gremlin/motor/omega_to_pwm/omega",__LINE__);
    }

    if (!nh.getParam("/asl_gremlin/motor/omega_to_pwm/pwm",  pwm_lookup_))
    {
        utility_pkg::throw_error_and_shutdown("/asl_gremlin/motor/omega_to_pwm/pwm",__LINE__);
    }

    std::string ang_vel_topic;
    if (!nh.getParam("/asl_gremlin/controller/cmd_angular_vel_topic",
                       ang_vel_topic))
    {
        utility_pkg::throw_error_and_shutdown("/asl_gremlin/controller/cmd_angular_vel_topic",
                                              __LINE__);
    }

    pwm_cmd_ = new asl_gremlin_msgs::MotorPwm();
    omega_cmd_ = new asl_gremlin_pkg::SubscribeTopic<asl_gremlin_msgs::MotorAngVel>(nh, ang_vel_topic,20);
}

OmegaToPWM::~OmegaToPWM()
{
    delete pwm_cmd_;
    delete omega_cmd_;
}

asl_gremlin_msgs::MotorPwm*
OmegaToPWM::convert_omega_to_pwm()
{
    ros::spinOnce();
   int left_idx = get_lower_index(omega_lookup_, (omega_cmd_->get_data())->wl);
   pwm_cmd_->pwm_l  = linear_interpolate(pwm_lookup_, omega_lookup_, left_idx, (omega_cmd_->get_data())->wl);
   
   int right_idx = get_lower_index(omega_lookup_, (omega_cmd_->get_data())->wr);
   pwm_cmd_->pwm_r  = linear_interpolate(pwm_lookup_, omega_lookup_, right_idx, (omega_cmd_->get_data())->wr);
   
   pwm_cmd_->header = (omega_cmd_->get_data())->header;

   return pwm_cmd_;
}

