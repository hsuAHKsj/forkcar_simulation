#include "agv_robot/control.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "control");

  ControlNS::Control control;
  control.MainLoop();

  return 0;
}
