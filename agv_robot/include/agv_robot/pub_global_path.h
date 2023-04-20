#ifndef PUB_GLOBAL_PATH_H_
#define PUB_GLOBAL_PATH_H_

#include "ros/ros.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>

#include <nav_msgs/Path.h>
#include <std_msgs/Int32.h>

#include <tf/tf.h>
#include <tf/transform_listener.h>

struct Points //读取全局路径
{  
  double x;
  double y;
  double yaw;
};

namespace PubGlobalPathNS
{
  class PubGlobalPath 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;

      ros::Subscriber number_sub_;

      ros::Publisher global_path_pub_;

      void Excution();
      void pubGlobalPath();
      void numberCallback(const std_msgs::Int32ConstPtr& msg);

    public:
      PubGlobalPath();
      ~PubGlobalPath();
      
      void MainLoop();

    private:
      int global_path_num_;

      int number_;//第几条线路
      int current_path_number_;
      int last_path_num_;

      bool is_pub_global_path_;

      //提外参
      bool is_loop_excution_;//循环执行

      std::vector<Points> vec_points_; //路径点存储
  };
}

#endif
