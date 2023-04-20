#include "agv_robot/obstacle_avoidance.h"

namespace ObstacleAvoidanceNS
{
  ObstacleAvoidance::ObstacleAvoidance() 
  {

  }

  ObstacleAvoidance::~ObstacleAvoidance()
  {

  }

  void ObstacleAvoidance::MainLoop()
  {
    ros::Rate loop_rate(20);

    while (ros::ok())
    {   
      ros::spinOnce();
      loop_rate.sleep();
    }
  }

}

