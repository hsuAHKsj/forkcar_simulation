#ifndef PUB_POINT_H_
#define PUB_POINT_H_

#include "ros/ros.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>

#include <nav_msgs/Path.h>

#include <tf/tf.h>
#include <tf/transform_listener.h>

struct Points //读取全局路径
{  
  double x;
  double y;
  double yaw;
};

namespace PubPointNS
{
  class PubPoint 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;

      ros::Publisher point_pub_;
      
      void initial();
      void pubPoint();
      void Excution();

    public:
      PubPoint();
      ~PubPoint();
      
      void MainLoop();

    private:
      int point_num_;

      std::vector<Points> vec_points_; //路径点存储    
  };
}

#endif
