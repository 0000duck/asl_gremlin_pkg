#ifndef _trajectory_generation_MINIMUMJERKTRAJECTORY_H_
#define _trajectory_generation_MINIMUMJERKTRAJECTORY_H_

#include <iostream>
#include "TrajectoryBase.h"
#include <array>
#include <trajectory_generation/ref_traj.h>
#include <ros/ros.h>
#include <ros/time.h>
#include "EvaluatePolynomial.h"

#define arr_size(x) std::tuple_size<decltype(x)>::value

template<typename T>
class MinimumJerkTrajectory : 
                    public TrajectoryBase {
    public:
        MinimumJerkTrajectory(T*);
        ~MinimumJerkTrajectory();
        void update_start_time(double) override;
        void set_ini_pose(double = 0.0,double = 0.0,double = 0.0) override;
        void set_final_pose(double,double,double = 0.0) override;
        void set_current_traj_value_to_ini() override;
        
        void calc_coeff() override;
        void generate_traj(double) override;

        trajectory_generation::ref_traj* get_trajectory() override;

    private:
        T* params_ = nullptr;

        std::array<double, 6> x_coeff_;
        std::array<double, 6> y_coeff_;

        double x_ini_ = 0.0, y_ini_ = 0.0;
        double x_final_ = 0.0, y_final_ = 0.0;
        double t_initial_ = 0.0, t_final_ = 0.0;
        
        trajectory_generation::ref_traj ref_traj_obj_;
        void calc_x_coeff_(double);
        void calc_y_coeff_(double);
        int msg_count = 0;
};

template<typename T>
MinimumJerkTrajectory<T>::MinimumJerkTrajectory(T* params)
{ params_ = params; }

template<typename T>
MinimumJerkTrajectory<T>::~MinimumJerkTrajectory()
{ delete params_;}

template<typename T>
void MinimumJerkTrajectory<T>::update_start_time(double t_initial)
{ t_initial_ = t_initial; }


template<typename T>
void MinimumJerkTrajectory<T>::set_ini_pose(    double x_ini,
                                                double y_ini, 
                                                double theta_ini)
{
    x_ini_ = x_ini;
    y_ini_ = y_ini;
}

template<typename T>
void MinimumJerkTrajectory<T>::set_final_pose(  double x_final, 
                                                double y_final,
                                                double theta_final)
{
    x_final_ = x_final;
    y_final_ = y_final;
}

template<typename T>
void MinimumJerkTrajectory<T>::set_current_traj_value_to_ini()
{
    x_ini_ = ref_traj_obj_.x;
    y_ini_ = ref_traj_obj_.y;
}

template<typename T> 
void MinimumJerkTrajectory<T>::calc_coeff()
{
    // calculate final time required
   double delta_x = fabs(x_final_ - x_ini_);
   double delta_y = fabs(y_final_ - y_ini_);

   double t_final_x = sqrt(10/(sqrt(3)) * delta_x/params_->accel_max);
   double t_final_y = sqrt(10/(sqrt(3)) * delta_y/params_->accel_max);
    
   double t_final = std::max(t_final_x, t_final_y) + std::max(delta_x, delta_y);

   calc_x_coeff_(t_final);
   calc_y_coeff_(t_final);
}

template<typename T>
void MinimumJerkTrajectory<T>::generate_traj(double time)
{
    double t_rel = (time - t_initial_);

    ref_traj_obj_.x         = get_Nth_order_polynomial<5, arr_size(x_coeff_)>(t_rel, x_coeff_, orderOfDiff::position);
    ref_traj_obj_.x_dot     = get_Nth_order_polynomial<5, arr_size(x_coeff_)>(t_rel, x_coeff_, orderOfDiff::velocity);
    ref_traj_obj_.x_ddot    = get_Nth_order_polynomial<5, arr_size(x_coeff_)>(t_rel, x_coeff_, orderOfDiff::acceleration);

    ref_traj_obj_.y         = get_Nth_order_polynomial<5, arr_size(y_coeff_)>(t_rel, y_coeff_, orderOfDiff::position);
    ref_traj_obj_.y_dot     = get_Nth_order_polynomial<5, arr_size(y_coeff_)>(t_rel, y_coeff_, orderOfDiff::velocity);
    ref_traj_obj_.y_ddot    = get_Nth_order_polynomial<5, arr_size(y_coeff_)>(t_rel, y_coeff_, orderOfDiff::acceleration);

    ref_traj_obj_.theta      =   std::atan2(ref_traj_obj_.y_dot, ref_traj_obj_.x_dot);
    ref_traj_obj_.theta_dot  =   0;
    ref_traj_obj_.theta_ddot =   0;

    ref_traj_obj_.header.seq = msg_count;
    ref_traj_obj_.header.stamp = ros::Time::now();

    msg_count++;
}

template<typename T>
trajectory_generation::ref_traj* MinimumJerkTrajectory<T>::get_trajectory()
{
    return &ref_traj_obj_;
}

template<typename T>
void MinimumJerkTrajectory<T>::calc_x_coeff_(double t_final)
{
    x_coeff_[0] = x_ini_;
    x_coeff_[3] = 10*(x_final_ - x_ini_)/(std::pow(t_final,3)); 
    x_coeff_[4] = -15*(x_final_ - x_ini_)/(std::pow(t_final,4));
    x_coeff_[5] = 6*(x_final_ - x_ini_)/(std::pow(t_final,5));
}

template<typename T>
void MinimumJerkTrajectory<T>::calc_y_coeff_(double t_final)
{
    y_coeff_[0] = y_ini_;
    y_coeff_[3] = 10*(y_final_ - y_ini_)/(std::pow(t_final,3)); 
    y_coeff_[4] = -15*(y_final_ - y_ini_)/(std::pow(t_final,4));
    y_coeff_[5] = 6*(y_final_ - y_ini_)/(std::pow(t_final,5));
}

#endif
