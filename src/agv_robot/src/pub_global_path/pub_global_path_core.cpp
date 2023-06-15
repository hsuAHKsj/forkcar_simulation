#include "agv_robot/pub_global_path.h"

namespace PubGlobalPathNS
{
  PubGlobalPath::PubGlobalPath() : is_pub_global_path_(false)
  {
    nh_.getParam("is_loop_excution_", is_loop_excution_);

    global_path_pub_ = nh_.advertise<nav_msgs::Path>("/global_path", 1);
    number_sub_ = nh_.subscribe("/set_path_number", 1, &PubGlobalPath::numberCallback, this);
  }

  PubGlobalPath::~PubGlobalPath()
  {
  
  }

  void PubGlobalPath::numberCallback(const std_msgs::Int32ConstPtr& msg)
  {  
      current_path_number_ = msg->data;
      std::cout << "current_path_number_ " << current_path_number_ << std::endl;
      if(last_path_num_ != current_path_number_ && current_path_number_ > 0)
      {
        number_ = current_path_number_;

        std::string str1 = "/home/nvidia/Desktop/BL_fork/src/path/path_";
        std::string str2 = std::to_string(number_);
        std::string str3 = ".csv";
        std::string str4 = str1 + str2 + str3;

        std::cout << str4 << std::endl;

        //读取保存的路径点
        std::ifstream data(str4);
        if(!data)
        {
          std::cout << "打开文件失败！" << std::endl;
          return;
        }
        else
        {
          std::cout << "发布线路：" << number_ << std::endl;

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
          
          Excution();

          is_pub_global_path_ = true;
        }
      }
      last_path_num_ = current_path_number_;
      std::cout << "last_path_num_ " << last_path_num_ << std::endl;
  }

  void PubGlobalPath::Excution()
  {
    if(is_loop_excution_)//循环绕圈
    {
      double start_to_end_dis = sqrt(pow(vec_points_.front().y - vec_points_.back().y, 2) 
                                   + pow(vec_points_.front().x - vec_points_.back().x, 2));
      std::cout << "start_to_end_dis " << start_to_end_dis << std::endl;

      if(start_to_end_dis <= 1.0 && vec_points_.size() >= 3)
      {
        std::cout << "起点和终点在1米范围内,可以执行重复绕圈" << std::endl;

        for (int i = 0; i < 3; i++)
        {
          double _x = vec_points_[i].x;
          double _y = vec_points_[i].y;
          double _yaw = vec_points_[i].yaw;
          Points pre_waypoints = {_x, _y, _yaw};  //放入结构体中
          vec_points_.push_back(pre_waypoints);
        }
        global_path_num_ = vec_points_.size();

        std::cout << "global_path_num_ " << global_path_num_ << std::endl;
        for(int i = 0; i < global_path_num_; i++)
        {
          std::cout << i << " x " << vec_points_[i].x << " y " << vec_points_[i].y 
                    << " yaw " << vec_points_[i].yaw << std::endl;
        }
      }
      else
      {
        std::cout << "起点和终点大于1米范围,不可以执行重复绕圈" << std::endl;
        std::cout << "可以从一下三个方面排查问题：" << std::endl;
        std::cout << "1、请重新确认行驶路径起点和终点是否在1米范围内" << std::endl;
        std::cout << "2、请重新确认前视距离是否过小" << std::endl;
        std::cout << "3、请重新确认路径数量是否满足" << std::endl;
      }
    }
    else//执行一次
    {
      std::cout << "执行一次模式" << std::endl;

      global_path_num_ = vec_points_.size();
      std::cout << "global_path_num_ " << global_path_num_ << std::endl;
      for(int i = 0; i < global_path_num_; i++)
      {
        std::cout << i << " x " << vec_points_[i].x << " y " << vec_points_[i].y 
                  << " yaw " << vec_points_[i].yaw << std::endl;
      }
    }
  }

  void PubGlobalPath::pubGlobalPath()
  {
    nav_msgs::Path path;
    path.header.stamp = ros::Time::now();
    path.header.frame_id = "map";
    path.poses.clear();

    for(int i = 0; i < global_path_num_; i++)
    {
      geometry_msgs::PoseStamped pose_stamped;
      pose_stamped.header.stamp = ros::Time::now();
      pose_stamped.header.frame_id = "map";
      pose_stamped.pose.position.x = vec_points_[i].x;
      pose_stamped.pose.position.y = vec_points_[i].y;
      pose_stamped.pose.orientation = tf::createQuaternionMsgFromYaw(vec_points_[i].yaw);
      path.poses.push_back(pose_stamped);
    }
    global_path_pub_.publish(path);
  }

  void PubGlobalPath::MainLoop()
  {
    ros::Rate loop_rate(20);

    while (ros::ok())
    {
      if(is_pub_global_path_)
      {
        pubGlobalPath();
      }

      ros::spinOnce();
      loop_rate.sleep();
    }
  }

}

