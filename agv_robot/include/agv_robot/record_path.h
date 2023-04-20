#ifndef RECORD_PATH_H_
#define RECORD_PATH_H_

#include <ros/ros.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Quaternion.h>
#include "nav_msgs/Odometry.h"
#include <tf/transform_datatypes.h>
#include <tf/tf.h>
#include <geometry_msgs/Twist.h>
#include "std_msgs/Int32.h"
#include <fstream>
#include <vector>
#include "math.h"
#include <string>

namespace RecordPathNS
{
  class RecordPath
  {
    protected:
      ros::NodeHandle nh_;

      ros::Subscriber odom_sub_;
      ros::Subscriber number_sub_;

      void odomCallback(const nav_msgs::OdometryConstPtr& msg);//里程计
      void numberCallback(const std_msgs::Int32ConstPtr& msg);
      void run();

    public:
      RecordPath();
      ~RecordPath();
      
      void MainLoop();

    private:
      int path_num_;//第几个路径点
      int number_;//第几条线路

      double current_pos_x_;//机器人当前位置
      double current_pos_y_;
      double current_pos_yaw_;

      double pre_pos_x_;//机器人的上一个位置
      double pre_pos_y_;
      double pre_pos_yaw_;

      bool is_number_sub_;
      bool is_record_;
      bool is_pose_sub_;
      bool receive_once_;

      int current_path_number_;
      int last_path_num_;

      std::ofstream outFile_;

  };
}

#endif



