#ifndef POINT2POINT_H_
#define POINT2POINT_H_

#include <geometry_msgs/Twist.h>
#include <nav_msgs/Path.h>
#include <ros/ros.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/Odometry.h"

#include "agv_robot/car_info.h" //小车状态

namespace Point2PointNS
{
  enum State //小车的状态
  {
    PURE_PURSUIT = 1,    //纯跟踪
    CORRECT_THE_LINE,    //纠正线路
    CAR_STOP,            //正常停车
    SCAN_STOP,           //雷达避障停车
    ROTATION,            //原地旋转
    ARRIVE_GOAL          //到达目标点
  };

  class Point2Point 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;

      ros::Subscriber point_sub_;
      ros::Subscriber odom_sub_;

      ros::Publisher cmd_vel_pub_;
      ros::Publisher car_info_pub_;

      void initial();
      void pointCallback(const nav_msgs::Path &msg);
      void odomCallback(const nav_msgs::Odometry &msg);
      inline double thetaLimit(double yaw);//角度限制
      inline double transformYawToAngle(double yaw);//角度转换
      void pubCarInfo();//发布小车状态信息
      void run();

      double Xianfu(double value, double Amplitude);//限制最大速度幅度
      double Incremental_PI_A(double Velocity, double Target);//增量PI控制器
      double Incremental_PI_B(double Velocity, double Target);//增量PI控制器
      double Position_PID_A(double position, double target);//位置式PID控制器
      double Position_PID_B(double Position, double target);//位置式PID控制器

    public:
      Point2Point();
      ~Point2Point();
      
      void MainLoop();

    private:
      nav_msgs::Path path_;

      std::vector<double> r_x_;
      std::vector<double> r_y_;

      double k_;
      double preview_dis_;
      double current_pose_x_;
      double current_pose_y_;
      double current_pose_yaw_;

      int point_num_;  //保存路径点的个数
      int target_index_;
      int index_;

      bool is_point_sub_;
      bool is_current_pose_sub_;
      bool is_rotation_finish_;//原地旋转完成标志位

      geometry_msgs::Twist current_velocity_;//小车当前的速度

      State car_state_;//实例化

      //加载的参数
      double car_linear_velocity_; //小车线速度
      double car_angular_velocity_; //小车角速度
      bool is_loop_excution_;//循环执行 

      //加载的参数
      double Velocity_KP_A; 
      double Velocity_KI_A;
      double Velocity_KD_A;//速度PID系数
      double Velocity_KP_B; 
      double Velocity_KI_B;
      double Velocity_KD_B;//速度PID系数

      double Position_KP_A; 
      double Position_KI_A; 
      double Position_KD_A;//位置PID系数
      double Position_KP_B; 
      double Position_KI_B; 
      double Position_KD_B;//位置PID系数  
  };
}

#endif
