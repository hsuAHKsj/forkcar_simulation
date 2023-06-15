#include "agv_robot/check_point.h"

namespace CheckPointNS
{
  CheckPoint::CheckPoint() : is_pose_sub_(false)
  {
    record_num_ = 0;
    
    odom_sub_ = nh_.subscribe("/odom", 10, &CheckPoint::odomCallback, this);//里程计

    check_point_thread_ = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&CheckPoint::checkPoint, this)));//创建线程
  }

  CheckPoint::~CheckPoint()
  {
    outFile_.close();
  }

  void CheckPoint::odomCallback(const nav_msgs::OdometryConstPtr& msg)//里程计
  {
    current_pos_x_ = msg->pose.pose.position.x;
    current_pos_y_ = msg->pose.pose.position.y;
    current_pos_yaw_ = tf::getYaw(msg->pose.pose.orientation);;

    is_pose_sub_ = true;
  }

  void CheckPoint::checkPoint() 
  {
    //ate的意思是程序启动会删除原来的文件,app的意思是在原来文件末尾添加新的文件
    outFile_.open("/home/huang/point.csv", std::ios::app);

    if (!outFile_)
    {
        std::cout << "打开文件失败！" << std::endl;
        return;
    }

    std::cout << "请按下键盘的“a”键记录位置点:" << std::endl;
    char buf[1024];
    char key_data[] = "a";
    ssize_t size = read(STDIN_FILENO, buf, sizeof(buf));

    ros::Rate loop_rate(1);

    while (ros::ok()) 
    {
      std::cin >> buf;
      std::cout << "键盘输入的数据为: " << buf << std::endl;

      if(strcmp(buf, key_data) == 0 && is_pose_sub_)
      {
        std::cout << "current_pos_x_ " << current_pos_x_ << std::endl;
        std::cout << "current_pos_y_ " << current_pos_y_ << std::endl;
        std::cout << "current_pos_yaw_ " << current_pos_yaw_ << std::endl;

        outFile_ << current_pos_x_ << "," << current_pos_y_ << "," << current_pos_yaw_ << std::endl;

        ++record_num_;
        std::cout << "成功记录 " << record_num_ << " 个位置点" << std::endl;

        is_pose_sub_ = false;
      }

      ros::spinOnce();
      loop_rate.sleep();
    }
  }

  void CheckPoint::MainLoop()
  {
    ros::Rate loop_rate(20);

    while (ros::ok())
    {   
      ros::spinOnce();
      loop_rate.sleep();
    }
  }

}

