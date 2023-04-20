#include "agv_robot/pub_point.h"

namespace PubPointNS
{
  PubPoint::PubPoint() 
  {
    initial();
    Excution();

    point_pub_ = nh_.advertise<nav_msgs::Path>("/point_path", 1);
  }

  PubPoint::~PubPoint()
  {

  }

  void PubPoint::initial()
  {
    //读取保存的路径点
    std::ifstream data("/home/huang/point.csv");
    if(!data)
    {
        std::cout << "打开文件失败！" << std::endl;
        return;
    }
    else
    {
      std::string line;
      std::vector<std::string> line_data;
      while (getline(data, line))
      { 
        std::stringstream lineStream(line);
        std::string cell;
        while(std::getline(lineStream, cell,','))
        {
          line_data.push_back(cell);
        }
      }

      vec_points_.clear();
      for (int i = 0; i < line_data.size(); i += 3)
      {
        double _x = std::atof(line_data[i + 0].c_str());
        double _y = std::atof(line_data[i + 1].c_str());
        double _yaw = std::atof(line_data[i + 2].c_str());
        Points pre_waypoints = {_x, _y, _yaw};  //放入结构体中
        vec_points_.push_back(pre_waypoints);
      }
    }
  }

  void PubPoint::Excution()
  {
    point_num_ = vec_points_.size();
    std::cout << "point_num_ " << point_num_ << std::endl;
    for(int i = 0; i < point_num_; i++)
    {
      std::cout << i << " x " << vec_points_[i].x << " y " << vec_points_[i].y 
                << " yaw " << vec_points_[i].yaw << std::endl;
    }
  }

  void PubPoint::pubPoint()
  {
    nav_msgs::Path path;
    path.header.stamp = ros::Time::now();
    path.header.frame_id = "map";
    path.poses.clear();

    for(int i = 0; i < point_num_; i++)
    {
      geometry_msgs::PoseStamped pose_stamped;
      pose_stamped.header.stamp = ros::Time::now();
      pose_stamped.header.frame_id = "map";
      pose_stamped.pose.position.x = vec_points_[i].x;
      pose_stamped.pose.position.y = vec_points_[i].y;
      pose_stamped.pose.orientation = tf::createQuaternionMsgFromYaw(vec_points_[i].yaw);
      path.poses.push_back(pose_stamped);
    }
    point_pub_.publish(path);
  }

  void PubPoint::MainLoop()
  {
    ros::Rate loop_rate(20);

    while (ros::ok())
    {   
      pubPoint();

      ros::spinOnce();
      loop_rate.sleep();
    }
  }

}

