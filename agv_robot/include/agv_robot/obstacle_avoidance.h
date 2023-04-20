#ifndef OBSTACLE_AVOIDANCE_H_
#define OBSTACLE_AVOIDANCE_H_

#include "ros/ros.h"

#include <iostream>

namespace ObstacleAvoidanceNS
{
  class ObstacleAvoidance 
  {
    protected:
      //ROS messages (topics)
      ros::NodeHandle nh_;
      ros::NodeHandle private_nh_;

    public:
      ObstacleAvoidance();
      ~ObstacleAvoidance();
      
      void MainLoop();

    private:
      
  };
}

#endif
