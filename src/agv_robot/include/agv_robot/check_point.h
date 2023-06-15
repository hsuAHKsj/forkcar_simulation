#ifndef CHECK_POINT_H_
#define CHECK_POINT_H_

#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Path.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include "geometry_msgs/PoseStamped.h"
#include "nav_msgs/Odometry.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <math.h>

namespace CheckPointNS
{
  class CheckPoint 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;

      ros::Subscriber odom_sub_;//接收里程计

      void odomCallback(const nav_msgs::OdometryConstPtr& msg);//里程计

    public:
      CheckPoint();
      ~CheckPoint();
      
      void MainLoop();

      void checkPoint();

    private:
      std::ofstream outFile_;

      double current_pos_x_, current_pos_y_, current_pos_yaw_;//记录当前位置
      int record_num_;//记录次数
      bool is_pose_sub_;

      boost::shared_ptr<boost::thread> check_point_thread_;//创建线程 
      
  };
}

#endif
