#include "agv_robot/control.h"

namespace ControlNS
{
  Control::Control() : is_car_info_sub_(false)
  {
    initial();

    car_info_sub_ = nh_.subscribe("/car_info_msg", 1, &Control::carInfoCallback, this);
    //cmd_vel_sub_ = nh_.subscribe("/cmd_vel", 20, &Control::speedCallback, this);//速度回调
    //scan_sub_ = nh_.subscribe("/scan", 20, &Control::scanCallback, this);//scan回调
 
    //odom_pub_ = nh_.advertise<nav_msgs::Odometry>("/odom", 10);  //发布里程计
  }

  Control::~Control()
  {

  }

  void Control::initial()
  {
    contact_num1_ = 0;//第1个对接点到达次数
    contact_num2_ = 0;//第2个对接点到达次数

    // vel_x_ = 0.0;//小车的线速度
    // vel_th_ = 0.0;//小车的角速度

    // left_change_flag_ = 4;//01 03 04 08
    // right_change_flag_ = 4;//01 03 04 08

    // left_high_ = 0;//left high
    // left_low_ = 0;//left low
    // right_high_ = 0;//right high
    // right_low_ = 0;//right low

    // vl_ = 0.0;
    // vr_ = 0.0;
    // width_robot_ = 0.21;//两个驱动轮之间的距离
    // wheel_radius_ = 0.1;//轮子半径

    // x_pillar_ = 0.0;//机器人距离物体在X轴上的距离
    // y_pillar_ = 0.0;//机器人距离物体在Y轴上的距离
    // alpha_pillar_ = 0.0;//机器人于物体之间的角度
    // smallest_distance_ = 1000.0;//定义了一个接收激光雷达扫描的距离

    // //里程计相关
    // x_ = 0.0;
    // y_ = 0.0;
    // th_ = 0.0;
    // dt_ = 0.0;

    // //定义串口的各种信息
    // serial::Timeout to = serial::Timeout::simpleTimeout(100);//创建timeout
    // sp_.setPort("/dev/ttyUSB0");//设置要打开的串口名称
    // sp_.setBaudrate(115200);//设置串口通信的波特率
    // sp_.setTimeout(to);//串口设置timeout
 
    // try//打开串口
    // {
    //   sp_.open();
    // }
    // catch(serial::IOException& e)
    // {
    //   std::cout << "无法打开串口" << std::endl;
    //   return;
    // }
 
    // if(sp_.isOpen())//判断串口是否打开成功
    // {
    //   std::cout << "打开串口成功" << std::endl;
    // }
    // else
    // {
    //   return;
    // }
  }

  void Control::carInfoCallback(const agv_robot::car_info& msg)
  {
    car_info_data_ = msg;
    //std::cout << car_info_data_ << std::endl;

    is_car_info_sub_ = true;
  }

  //TODO:判断哪个点是对接点，根据实际需求更改
  void Control::contactPoint()
  {
    //到达对接点1
    if(car_info_data_.car_state == 6 && car_info_data_.is_rotation_finish == true && car_info_data_.index == 4)
    {
      std::cout << "到达第1个对接点" << std::endl;
      ++contact_num1_;//记录次数
      std::cout << "第1个对接点到达次数为 " << contact_num1_ << std::endl;
    }
    else if(car_info_data_.car_state == 6 && car_info_data_.is_rotation_finish == true && car_info_data_.index == 9)
    {
      std::cout << "到达第2个对接点" << std::endl;
      ++contact_num2_;//记录次数
      std::cout << "第2个对接点到达次数为 " << contact_num2_ << std::endl;
    }
    // else
    // {
    //   std::cout << "行走中......" << std::endl;
    // }
  }

  void Control::speedCallback(const geometry_msgs::Twist &msg)//速度控制回调
  {
    geometry_msgs::Twist twist = msg;

    vel_x_ = msg.linear.x * 1.28;//单位：m/s
    vel_th_ = -msg.angular.z * 1.28;
    std::cout << "线速度 vel_x_ " << vel_x_ << " m/s";
    std::cout << "  角速度 vel_th_ " << vel_th_  << " m/s" << std::endl;

    double right_vel = 0.0;
    double left_vel = 0.0;

    if(vel_x_ == 0)//turning
    {
      right_vel = vel_th_ * width_robot_ / 2.0;
      left_vel = (-1) * right_vel;
    }
    else if(vel_th_ == 0)//forward / backward
    {
      left_vel = vel_x_;
      right_vel = vel_x_;
    }
    else//moving doing arcs
    {
      left_vel = vel_x_ - vel_th_ * width_robot_ / 2.0;
      right_vel = vel_x_ + vel_th_ * width_robot_ / 2.0;
    }

    vl_ = left_vel;
    vr_ = right_vel;

    if(vl_ > 0 && vr_ >0)
    {
      left_change_flag_ = 1;
      left_high_ = vl_ * (10 * 8 * 30 * 60) / (20 * M_PI * 0.1) / 255.0;
      left_low_ = (int)(vl_ * (10 * 8 * 30 * 60) / (20 * M_PI * 0.1)) % 255;
      right_change_flag_ = 1;
      right_high_ = vr_ * (10 * 8 * 30 * 60) / (20 * M_PI * 0.1) / 255.0;
      right_low_ = (int)(vr_ * (10 * 8 * 30 * 60) / (20 * M_PI * 0.1)) % 255;
      std::cout << "前进" << std::endl;
    }
    if(vl_ < 0 && vr_ < 0)
    {
      left_change_flag_ = 3;
      left_high_ = (-vl_) * (10 * 8 * 30 * 60) / (20 * M_PI * 0.1) / 255.0;
      left_low_ = (int)((-vl_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 3;
      right_high_ = (-vr_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)((-vr_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "后退" << std::endl;
    }
    if((vl_ < vr_) && vl_ > 0 && vr_ >0)
    {
      left_change_flag_ = 1;
      left_high_ = vl_*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      left_low_ = (int)(vl_*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 1;
      right_high_ = vr_*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)(vr_*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "前进+左转" << std::endl;
    }
    if((vl_ > vr_) && vl_ > 0 && vr_ >0)
    {
      left_change_flag_ = 1;
      left_high_ = vl_*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      left_low_ = (int)(vl_*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 1;
      right_high_ = vr_*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)(vr_*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "前进+右转" << std::endl;
    }
    if((vl_ < vr_) && vl_ < 0 && vr_ < 0)
    {
      left_change_flag_ = 3;
      left_high_ = (-vl_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      left_low_ = (int)((-vl_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 3;
      right_high_ = (-vr_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)((-vr_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "后退+左转" << std::endl;
    }
    if((vl_ > vr_) && vl_ < 0 && vr_ < 0)
    {
      left_change_flag_ = 3;
      left_high_ = (-vl_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      left_low_ = (int)((-vl_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 3;
      right_high_ = (-vr_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)((-vr_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "后退+右转" << std::endl;
    }
    if(vl_ < 0 && vr_ > 0 && ((-vl_) == vr_))
    {
      left_change_flag_ = 3;
      left_high_ = (-vl_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      left_low_ = (int)((-vl_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 1;
      right_high_ = vr_*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)(vr_*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "原地逆时针左转" << std::endl;
    }
    if(vl_ > 0 && vr_ < 0 && (vl_ == (-vr_)))
    {
      left_change_flag_ = 1;
      left_high_ = vl_*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      left_low_ = (int)(vl_*(10*8*30*60)/(20*M_PI*0.1))%255;
      right_change_flag_ = 3;
      right_high_ = (-vr_)*(10*8*30*60)/(20*M_PI*0.1)/255.0;
      right_low_ = (int)((-vr_)*(10*8*30*60)/(20*M_PI*0.1))%255;
      std::cout << "原地逆时针右转" << std::endl;
    }
    if(vl_ == 0 && vr_ == 0)
    {
      left_change_flag_ = 4;
      left_high_ = 0;
      left_low_ = 0;
      right_change_flag_ = 4;
      right_high_ = 0;
      right_low_ = 0;
      std::cout << "停止" << std::endl;
    }

    Txbuff_[0] = 0xAA;
    Txbuff_[1] = 0x55;
    Txbuff_[2] = left_change_flag_;
    Txbuff_[3] = left_high_;
    Txbuff_[4] = left_low_;
    Txbuff_[5] = right_change_flag_;
    Txbuff_[6] = right_high_;
    Txbuff_[7] = right_low_;
    Txbuff_[8] = 0x00;
    Txbuff_[9] = 0x00;
    Txbuff_[10] = 0x00;
    Txbuff_[11] = 0x00;
    Txbuff_[12] = 0xFF;
    Txbuff_[13] = 0x88;

    sp_.write(Txbuff_, 14);//把数据发送到串口
  }

  void Control::scanCallback(const sensor_msgs::LaserScan &msg)
  {
    //TODO：预留给反光柱定位使用或者雷达避障

    //使用arr_size接收激光雷达扫描一次的激光点数（(最大角度-最小角度)/单位角度 = 激光点的个数）
    int arr_size = floor((msg.angle_max-msg.angle_min) / msg.angle_increment);
    //cout << "arr_size: " << arr_size << endl;//810
    //for语句来找到扫描一次后离机器人最近的直线距离的点
    //for(int i = 110; i < 700; i++) //滤波
    for(int i = 0; i < arr_size; i++) 
    {
      //cout << " " << i << " " << msg.ranges[i] << endl;
      if(msg.ranges[i] < 0.15) //滤波
      {
        continue;
      }
      if(msg.ranges[i] < smallest_distance_) 
      {
        smallest_distance_ = msg.ranges[i];
        //并计算出角弧度
        alpha_pillar_ = (msg.angle_min + i * msg.angle_increment);
      }
    }
    //cout << "smallest_distance_: " << smallest_distance_ << endl;
    //通过得到的直线距离点，再通过三角函数公式，算出物体在以机器人为原点的X轴和Y轴的偏移量
    x_pillar_ = -smallest_distance_ * cos(alpha_pillar_);
    y_pillar_ = smallest_distance_ * sin(alpha_pillar_);

    if(smallest_distance_ <= 1.0)//雷达避障的距离
    {
      std::cout << "小车停止" << std::endl;
      //current_vel_x = 0.0; //小车停止
    } 
    else 
    {
      std::cout << "小车前进" << std::endl;
      //current_vel_x = speed_vel_x; //小车前进
    }
    smallest_distance_ = 1000;
  }

  void Control::calOdom() //计算里程计，回调底盘数据
  {
    //差速轮里程计计算参考：https://blog.csdn.net/u012926144/article/details/80787175
    //左轮回调速度 + 右轮回调速度
    //右轮回调速度 - 左轮回调速度
    double odom_vel_x = (0.0 + 0.0) / 2.0;
    double odom_vel_th = (0.0 - 0.0) / wheel_radius_  / 10.0 * 1.72;
    //printf("vel_x = %f vel_th = %f \n", vel_x, vel_th);

    //里程计计算
    x_ = x_ + odom_vel_x * cos(th_) * dt_;
    y_ = y_ + odom_vel_x * sin(th_) * dt_;
    //printf("x = %f y = %f th = %f\n", x, y, th);
  }

  void Control::pubOdom()   //发布里程计
  {
    geometry_msgs::TransformStamped odom_trans;  //消息声明
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    geometry_msgs::Quaternion odom_quat;

    odom_quat = tf::createQuaternionMsgFromRollPitchYaw(0, 0, th_);

    odom_trans.header.stamp = ros::Time::now();  // update
    odom_trans.transform.translation.x = x_;
    odom_trans.transform.translation.y = y_;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(th_);

    nav_msgs::Odometry odom;  // filling the odometry
    odom.header.stamp = ros::Time::now();
    odom.header.frame_id = "odom";
    odom.child_frame_id = "base_link";

    odom.pose.pose.position.x = x_;  // position
    odom.pose.pose.position.y = y_;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    odom.twist.twist.linear.x = vel_x_;  // velocity
    odom.twist.twist.linear.y = 0.0;
    odom.twist.twist.linear.z = 0.0;
    odom.twist.twist.angular.x = 0.0;
    odom.twist.twist.angular.y = 0.0;
    odom.twist.twist.angular.z = vel_th_;

    broadcaster_.sendTransform(odom_trans);  // publishing the odometry and the new tf

    odom_pub_.publish(odom);
  }

  void Control::MainLoop()
  {
    ros::Time current_time;
    ros::Time last_time;
    current_time = ros::Time::now();
    last_time = ros::Time::now();

    ros::Rate loop_rate(20);

    while (ros::ok())
    {   
      if(is_car_info_sub_)
      {
        contactPoint();

        is_car_info_sub_ = false;
      }

      // sp_.read(Rxbuff_, 14);//读取数据

      // current_time = ros::Time::now();
      // dt_ = (current_time - last_time).toSec();
      // last_time = current_time;

      // calOdom(); //计算里程计
      // pubOdom(); //发布里程计

      ros::spinOnce();
      loop_rate.sleep();
    }

    //sp_.close();//关闭串口
  }

}

