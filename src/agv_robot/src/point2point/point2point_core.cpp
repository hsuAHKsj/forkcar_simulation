#include "agv_robot/point2point.h"

namespace Point2PointNS
{
  Point2Point::Point2Point() : is_point_sub_(false), is_current_pose_sub_(false), is_rotation_finish_(false) 
  {
    nh_.getParam("car_linear_velocity_", car_linear_velocity_);
    nh_.getParam("car_angular_velocity_", car_angular_velocity_);
    nh_.getParam("is_loop_excution_", is_loop_excution_);
    nh_.getParam("Velocity_KP_A", Velocity_KP_A);//PID参数加载
    nh_.getParam("Velocity_KI_A", Velocity_KI_A);
    nh_.getParam("Velocity_KD_A", Velocity_KD_A);
    nh_.getParam("Velocity_KP_B", Velocity_KP_B);
    nh_.getParam("Velocity_KI_B", Velocity_KI_B);
    nh_.getParam("Velocity_KD_B", Velocity_KD_B);
    nh_.getParam("Position_KP_A", Position_KP_A);
    nh_.getParam("Position_KI_A", Position_KI_A);
    nh_.getParam("Position_KD_A", Position_KD_A);
    nh_.getParam("Position_KP_B", Position_KP_B);
    nh_.getParam("Position_KI_B", Position_KI_B);
    nh_.getParam("Position_KD_B", Position_KD_B);

    initial();//初始化

    point_sub_ = nh_.subscribe("/point_path", 1, &Point2Point::pointCallback, this);
    odom_sub_ = nh_.subscribe("/odom", 10, &Point2Point::odomCallback, this);

    cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    car_info_pub_ = nh_.advertise<agv_robot::car_info>("/car_info_msg", 1);
  }

  Point2Point::~Point2Point()
  {

  }

  void Point2Point::initial()
  {
    k_ = 0.1;
    point_num_ = 0;  //保存路径点的个数
    target_index_ = point_num_ - 1;
    index_ = 0;//初始化，从原点开始
  }

  //计算发送给模型车的转角
  void Point2Point::odomCallback(const nav_msgs::Odometry &msg) 
  {
    current_pose_x_ = msg.pose.pose.position.x;
    current_pose_y_ = msg.pose.pose.position.y;
    current_pose_yaw_ = tf::getYaw(msg.pose.pose.orientation);

    current_velocity_ = msg.twist.twist;

    is_current_pose_sub_ = true;
  }

  void Point2Point::pointCallback(const nav_msgs::Path &msg) 
  {
    point_num_ = msg.poses.size();
    //std::cout << "point_num_ " << point_num_ << std::endl;

    r_x_.clear();
    r_y_.clear();
    for (int i = 0; i < point_num_; i++) 
    {
      r_x_.push_back(msg.poses[i].pose.position.x);
      r_y_.push_back(msg.poses[i].pose.position.y);
    }

    is_point_sub_ = true;
  }

  inline double Point2Point::thetaLimit(double yaw)//角度限制 0~180度，-180度~0
  {
    double theta = yaw;
    if(theta > M_PI)
    {
      theta = theta - 2 * M_PI;
    }
    else if(theta < -M_PI)
    {
      theta = theta + 2 * M_PI;
    }
    return theta;
  } 

  inline double Point2Point::transformYawToAngle(double yaw)//角度转换，0度~360
  {
    double angle = yaw;
    if(angle < 0.0)
    {
      angle = angle + 2 * M_PI;
    }
    return angle;
  }

  void Point2Point::pubCarInfo()//发布小车状态信息
  {
    agv_robot::car_info car_info_msg;
    car_info_msg.car_state = car_state_;
    car_info_msg.is_rotation_finish = is_rotation_finish_;
    car_info_msg.index = index_;

    car_info_pub_.publish(car_info_msg);
  }

  double Point2Point::Xianfu(double value, double Amplitude)//限制最大速度幅度
  {
    double temp;
    if(value > Amplitude)
      temp = Amplitude;
    else if(value < -Amplitude)
      temp = -Amplitude;
    else
      temp = value;

    return temp;
  }

  // 函数功能：增量PI控制器
  // 入口参数：当前速度，目标速度
  // 返回  值：控制的速度
  // 根据增量式离散PID公式 
  // Speed+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
  // e(k)代表本次偏差 
  // e(k-1)代表上一次的偏差  以此类推 
  // speed代表增量输出
  // 在我们的速度控制闭环系统里面，只使用PI控制
  // Speed+=Kp[e（k）-e(k-1)]+Ki*e(k)
  double Point2Point::Incremental_PI_A(double Velocity, double Target)
  {   
    static double Bias, Speed, Last_bias, Integral_bias;
    Bias = Target - Velocity;//计算偏差
    Integral_bias += Bias;
    Integral_bias = Xianfu(Integral_bias, 2.0);//速度最大限制在0.8
    Speed = Velocity_KP_A * Bias + Velocity_KI_A * Integral_bias + Velocity_KD_A *(Bias - Last_bias);//增量式PI控制器
    Last_bias = Bias;//保存上一次偏差 

    if(Speed > 2.0)
      Speed = 2.0;
    if(Speed < -2.0)
      Speed = -2.0;

    return Speed;//增量输出
  }

  double Point2Point::Incremental_PI_B(double Velocity, double Target)
  {   
    static double Bias, Speed, Last_bias, Integral_bias;
    Bias = Target - Velocity;//计算偏差
    Integral_bias += Bias;
    Integral_bias = Xianfu(Integral_bias, 2.0);//速度最大限制在0.5
    Speed = Velocity_KP_B * Bias + Velocity_KI_B * Integral_bias + Velocity_KD_B *(Bias - Last_bias);//增量式PI控制器
    Last_bias = Bias;//保存上一次偏差 

    if(Speed > 2.0)
      Speed = 2.0;
    if(Speed < -2.0)
      Speed = -2.0;

    return Speed;//增量输出
  }

  // 函数功能：位置式PID控制器
  // 入口参数：当前位置，目标位置
  // 返回  值：控制的速度
  // 根据位置式离散PID公式 
  // Speed=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
  // e(k)代表本次偏差 
  // e(k-1)代表上一次的偏差  
  // ∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
  // Speed代表输出
  double Point2Point::Position_PID_A(double Position, double target)
  {   
    static float Bias, Speed, Integral_bias, Last_Bias;
    Bias = target - Position;//计算偏差
    Integral_bias += Bias;//求出偏差的积分
    Integral_bias = Xianfu(Integral_bias, 0.5);
    Speed = Position_KP_A * Bias + Position_KI_A * Integral_bias + Position_KD_A * (Bias - Last_Bias);//位置式PID控制器
    Last_Bias = Bias;//保存上一次偏差 
    
    if(Speed > 0.5)
      Speed = 0.5;
    if(Speed < -0.5)
      Speed = -0.5;

    return Speed;//增量输出
  }

  double Point2Point::Position_PID_B(double Position, double target)
  {   
    static float Bias, Speed, Integral_bias, Last_Bias;
    Bias = target - Position;//计算偏差
    Integral_bias += Bias;//求出偏差的积分
    Integral_bias = Xianfu(Integral_bias, 0.5);
    Speed = Position_KP_B * Bias + Position_KI_B * Integral_bias + Position_KD_B * (Bias - Last_Bias);//位置式PID控制器
    Last_Bias = Bias;//保存上一次偏差 
    
    if(Speed > 0.5)
      Speed = 0.5;
    if(Speed < -0.5)
      Speed = -0.5;

    return Speed;//增量输出
  }

  void Point2Point::run()
  {
    std::cout << "目标点标号 " << index_ << std::endl;//索引号
    
    if(!is_rotation_finish_)//没有完成原地旋转
    {
      double angle = transformYawToAngle(atan2(r_y_[index_] - current_pose_y_, r_x_[index_] - current_pose_x_) - current_pose_yaw_);//0~360度
      //std::cout << "angle " << angle << std::endl;
      if(angle >= 5.0 * M_PI / 180.0 && angle <= 355.0 * M_PI / 180.0)//度转弧度
      {
        car_state_ = ROTATION;//小车状态：原地旋转

        geometry_msgs::Twist vel_msg;//发布速度
        vel_msg.linear.x = 0.0;
        double motor = Position_PID_A(-angle, 0.0);//位置PID控制器
        vel_msg.angular.z = Incremental_PI_A(current_velocity_.angular.z, motor);//增量PID控制器
        std::cout << "原地旋转的角速度 " << vel_msg.angular.z << std::endl;
        cmd_vel_pub_.publish(vel_msg); 

        is_rotation_finish_ = false; 
      } 
      else
      {
        car_state_ = ARRIVE_GOAL;//小车状态：到达目标点

        is_rotation_finish_ = true;
      }
    }
    else//原地旋转完成，继续执行跟线
    {
      double alpha = thetaLimit(atan2(r_y_[index_] - current_pose_y_, r_x_[index_] - current_pose_x_) - current_pose_yaw_);
      //std::cout << "alpha " << alpha << std::endl;//0~180度，-180度~0

      // 当前点和目标点的距离Id
      double dl = sqrt(pow(r_y_[index_] - current_pose_y_, 2) + pow(r_x_[index_] - current_pose_x_, 2));
      std::cout << "距离目标点的距离 " << dl << std::endl;

      double curvature_k = 2 * sin(alpha) / dl;//跟踪曲率 k = 2 * sin(a) / Ld
      //std::cout << "curvature_k " << curvature_k << std::endl;

      if(alpha >= 5.0 * M_PI / 180.0 || alpha <= -5.0 * M_PI / 180.0)//纠正线路
      {
        car_state_ = CORRECT_THE_LINE;//小车状态：纠正线路

        geometry_msgs::Twist vel_msg;//发布速度
        vel_msg.linear.x = 0.0;
        if(curvature_k >= 0.0)//区分往左边还是往右边掉头
        {
          vel_msg.angular.z = car_angular_velocity_;
        }
        else
        {
          vel_msg.angular.z = -car_angular_velocity_;
        }
        cmd_vel_pub_.publish(vel_msg); 
      }
      else
      {
        double theta = car_linear_velocity_ * curvature_k;
        //std::cout << "theta " << theta << std::endl;

        // 发布小车运动指令
        if(dl > 0.1) 
        {
          car_state_ = PURE_PURSUIT;//小车状态：纯跟踪跟线

          //增加串级PID控制线速度
          double motor = Position_PID_B(-dl, 0.0);//小车当前位置到目标位置的距离，距离逐渐增加
          geometry_msgs::Twist vel_msg;//发布速度
          vel_msg.linear.x = Incremental_PI_B(current_velocity_.linear.x, motor);
          vel_msg.angular.z = theta;
          std::cout << "跟线的线速度 " << vel_msg.linear.x;
          std::cout << "  角速度 " << vel_msg.angular.z << std::endl;
          cmd_vel_pub_.publish(vel_msg);
        } 
        else 
        { 
          ++index_;
          is_rotation_finish_ = false;
          if(index_ >= point_num_)
          {
            if(!is_loop_excution_)//执行一次即可
            {
              index_ = point_num_;

              car_state_ = CAR_STOP;//小车状态：正常停车

              geometry_msgs::Twist vel_msg;
              vel_msg.linear.x = 0;
              vel_msg.angular.z = 0;
              cmd_vel_pub_.publish(vel_msg);

              is_point_sub_ = false;   
            }
            else//循环执行
            {
              index_ = 0;

              is_point_sub_ = false;
            }
          }
        }
      }
    }
  }

  void Point2Point::MainLoop()
  {
    ros::Rate loop_rate(20);

    while (ros::ok())
    {
      if(is_point_sub_ && is_current_pose_sub_)
      {
        run();//主程序

        switch(car_state_)//状态机
        {
          case PURE_PURSUIT:
            std::cout << "小车状态：纯跟踪跟线" << std::endl;
            break;
          case CORRECT_THE_LINE:
            std::cout << "小车状态：纠正线路" << std::endl;
            break;
          case CAR_STOP:
            std::cout << "小车状态：正常停车" << std::endl;
            break; 
          case SCAN_STOP:
            std::cout << "小车状态：雷达避障停车" << std::endl;
            break;  
          case ROTATION:
            std::cout << "小车状态：原地旋转" << std::endl;
            break; 
          case ARRIVE_GOAL:
            std::cout << "小车状态：到达目标点" << std::endl;
            break;    
          default:
            break;
        }

        pubCarInfo();//发布小车状态信息

        is_current_pose_sub_ = false;
      } 

      ros::spinOnce();
      loop_rate.sleep();
    }
  }

}

