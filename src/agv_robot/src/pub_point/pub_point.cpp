#include "agv_robot/pub_point.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "pub_point");

  PubPointNS::PubPoint pub_point;
  pub_point.MainLoop();

  return 0;
}
