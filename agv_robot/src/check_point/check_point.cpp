#include "agv_robot/check_point.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "check_point");

  CheckPointNS::CheckPoint check_point;
  check_point.MainLoop();

  return 0;
}
