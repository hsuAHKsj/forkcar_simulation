#include "agv_robot/pub_global_path.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "pub_global_path");

  PubGlobalPathNS::PubGlobalPath pub_global_path;
  pub_global_path.MainLoop();

  return 0;
}
