#include "agv_robot/obstacle_avoidance.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "obstacle_avoidance");

  ObstacleAvoidanceNS::ObstacleAvoidance obstacle_avoidance;
  obstacle_avoidance.MainLoop();

  return 0;
}
