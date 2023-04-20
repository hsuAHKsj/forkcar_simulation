#include "agv_robot/point2point.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "point2point");

  Point2PointNS::Point2Point point2point;
  point2point.MainLoop();

  return 0;
}
