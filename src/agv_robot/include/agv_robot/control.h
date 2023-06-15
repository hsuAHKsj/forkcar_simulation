#ifndef CONTROL_H_
#define CONTROL_H_

#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <nav_msgs/Odometry.h>
#include <serial/serial.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctime>
#include "math.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "tf/transform_datatypes.h"
#include "tf/transform_listener.h"
#include <tf/transform_broadcaster.h>

#include <iostream>

#include "agv_robot/car_info.h" //小车状态

namespace ControlNS
{
  class Control 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;
      
      ros::Subscriber car_info_sub_;
      ros::Subscriber cmd_vel_sub_;
      ros::Subscriber scan_sub_;

      ros::Publisher odom_pub_;

      void carInfoCallback(const agv_robot::car_info& msg);
      void contactPoint();
      void initial();
      void speedCallback(const geometry_msgs::Twist &msg);//速度控制回调
      void scanCallback(const sensor_msgs::LaserScan &msg);
      void calOdom(); //计算里程计
      void pubOdom(); //发布里程计

    public:
      Control();
      ~Control();
      
      void MainLoop();

    private:
      agv_robot::car_info car_info_data_;

      bool is_car_info_sub_;

      int contact_num1_;
      int contact_num2_;

      serial::Serial sp_;//创建一个serial类
      unsigned char Txbuff_[12];//串口发送的消息
      unsigned char Rxbuff_[14];//串口接收的消息
      
      double vel_x_;//小车的线速度
      double vel_th_;//小车的角速度

      int left_change_flag_;//01 03 04 08
      int right_change_flag_;//01 03 04 08

      int left_high_;//left high
      int left_low_;//left low
      int right_high_;//right high
      int right_low_;//right low

      double vl_;
      double vr_;
      double width_robot_;
      double wheel_radius_;

      //雷达避障碍参数
      double x_pillar_;//机器人距离物体在X轴上的距离
      double y_pillar_;//机器人距离物体在Y轴上的距离
      double alpha_pillar_;//机器人于物体之间的角度
      double smallest_distance_;//定义了一个接收激光雷达扫描的距离

      //里程计相关
      double x_;
      double y_;
      double th_;
      double dt_;

      // variables
      tf::TransformBroadcaster broadcaster_;  //计算odom使用

      //tf::TransformListener listener_;  //监听底盘坐标系
  };
}

#endif
