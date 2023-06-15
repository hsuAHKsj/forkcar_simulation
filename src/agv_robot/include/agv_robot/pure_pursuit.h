#ifndef PURE_PURSUIT_H_
#define PURE_PURSUIT_H_

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
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/Int32.h"

namespace PurePursuitNS
{
  enum State //小车的状态
  {
    PURE_PURSUIT = 1,    //纯跟踪
    CORRECT_THE_LINE,    //纠正线路
    CAR_STOP,            //正常停车
    SCAN_STOP,           //雷达避障停车
    ROTATION,            //原地旋转
    CIRCUMVENT           //绕障
  };

  enum Circumvent //绕障状态
  {
    BACK = 1,           //后退
    SPIN,               //旋转
    ADVANCE,            //前进
    FINISH              //完成
  };
    
  class PurePursuit 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;

      ros::Subscriber global_path_sub_;
      ros::Subscriber odom_sub_;
      ros::Subscriber scan_sub_;
      ros::Subscriber sim_obstacle_sub_;

      ros::Publisher cmd_vel_pub_;

      void initial();
      void globalPathCallback(const nav_msgs::Path &msg);
      void odomCallback(const nav_msgs::Odometry &msg);
      void scanCallback(const sensor_msgs::LaserScanConstPtr &msg); //雷达回调
      void simObstacleCallback(const std_msgs::Int32ConstPtr& msg);//仿真障碍物回调
      int getIndex(double pose_x, double pose_y, double pose_yaw);
      inline double thetaLimit(double yaw);//角度限制
      void run();
      void circumventLeft();//向左绕障
      void circumventRight();//向右绕障

      double Xianfu(double value, double Amplitude);//限制最大速度幅度
      double Incremental_PI(double Velocity, double Target);//增量PI控制器
      double Position_PID(double position, double target);//位置式PID控制器
 
    public:
      PurePursuit();
      ~PurePursuit();
      
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

      bool is_global_path_sub_;
      bool is_current_pose_sub_;

      geometry_msgs::Twist current_velocity_;//小车当前的速度

      State car_state_;//实例化

      double x_pillar_;//机器人距离物体在X轴上的距离
      double y_pillar_;//机器人距离物体在Y轴上的距离
      double alpha_pillar_;//机器人于物体之间的角度
      double smallest_distance_;//定义了一个接收激光雷达扫描的距离

      //绕障相关参数
      int obstacle_num_;
      bool is_record_pose_;
      double record_pose_x_;
      double record_pose_y_;
      double record_pose_yaw_;
      bool is_distance_flag_;
      double distance_pose_x_;
      double distance_pose_y_;
      bool is_back_finish_;
      bool is_turn_finish_;
      int left_obs_num_;
      int right_obs_num_;

      Circumvent car_detour_;

      //加载的参数
      double car_linear_velocity_; //小车线速度
      double car_angular_velocity_; //小车角速度

      //更改预瞄距离可以改变机器人路径追踪的方式
      //较小的预瞄距离可以使机器人更加精确地追踪实际的路径，但是可能会使机器人在追踪路径时产生振荡，
      //较大的预瞄距离可以减少机器人沿路的振荡，但最追踪的路径可能不够精确，也会导致拐角处附近的曲率变大
      double preview_distance_; //预瞄距离，要比路径的步长要大一点，但是不能太大

      bool is_use_pid_param_;
      double Velocity_KP; 
      double Velocity_KI;
      double Velocity_KD;//速度PID系数
      double Position_KP; 
      double Position_KI; 
      double Position_KD;//位置PID系数  

      bool is_use_detour_;//是否使用绕障   
  };
}

#endif
