#include "agv_robot/pure_pursuit.h"

namespace PurePursuitNS
{
  PurePursuit::PurePursuit() : is_global_path_sub_(false), is_current_pose_sub_(false) 
  {
    nh_.getParam("car_linear_velocity_", car_linear_velocity_);
    nh_.getParam("car_angular_velocity_", car_angular_velocity_);
    nh_.getParam("preview_distance_", preview_distance_);
    nh_.getParam("is_use_pid_param_", is_use_pid_param_);
    nh_.getParam("is_use_detour_", is_use_detour_);
    nh_.getParam("Velocity_KP", Velocity_KP);//PID参数加载
    nh_.getParam("Velocity_KI", Velocity_KI);
    nh_.getParam("Velocity_KD", Velocity_KD);
    nh_.getParam("Position_KP", Position_KP);
    nh_.getParam("Position_KI", Position_KI);
    nh_.getParam("Position_KD", Position_KD);

    initial();//初始化

    global_path_sub_ = nh_.subscribe("/global_path", 1, &PurePursuit::globalPathCallback, this);
    odom_sub_ = nh_.subscribe("/odom", 10, &PurePursuit::odomCallback, this);
    scan_sub_ = nh_.subscribe("/scan", 20, &PurePursuit::scanCallback, this);//雷达回调
    if(is_use_detour_)
    {
        sim_obstacle_sub_ = nh_.subscribe("/sim_obstacle", 1, &PurePursuit::simObstacleCallback, this);
    }

    cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
  }

  PurePursuit::~PurePursuit()
  {

  }

  void PurePursuit::initial()
  {
    k_ = 0.1;
    preview_dis_ = k_ * car_linear_velocity_ + preview_distance_;
    point_num_ = 0;  //保存路径点的个数
    target_index_ = point_num_ - 1;

    //雷达避障碍参数
    x_pillar_ = 0.0;//机器人距离物体在X轴上的距离
    y_pillar_ = 0.0;//机器人距离物体在Y轴上的距离
    alpha_pillar_ = 0.0;//机器人于物体之间的角度
    smallest_distance_ = 10.0;//定义了一个接收激光雷达扫描的距离

    //雷达绕障参数
    obstacle_num_ = 0;
    is_record_pose_ = true;
    is_distance_flag_ = true;
    is_back_finish_ = false;
    is_turn_finish_ = false;
    left_obs_num_ = 0;
    right_obs_num_ = 0;
  }

  //计算发送给模型车的转角
  void PurePursuit::odomCallback(const nav_msgs::Odometry &msg) 
  {
    current_pose_x_ = msg.pose.pose.position.x;
    current_pose_y_ = msg.pose.pose.position.y;
    current_pose_yaw_ = M_PI + tf::getYaw(msg.pose.pose.orientation);

    current_velocity_ = msg.twist.twist;

    is_current_pose_sub_ = true;
  }

  void PurePursuit::globalPathCallback(const nav_msgs::Path &msg) 
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

    is_global_path_sub_ = true;
  }

  void PurePursuit::scanCallback(const sensor_msgs::LaserScanConstPtr &msg)
  {
    //使用arr_size接收激光雷达扫描一次的激光点数（(最大角度-最小角度)/单位角度 = 激光点的个数）
    int arr_size = floor((msg->angle_max - msg->angle_min) / msg->angle_increment);
    //cout << "arr_size: " << arr_size << endl;//810
    //for语句来找到扫描一次后离机器人最近的直线距离的点
    //for(int i = 110; i < 700; i++) //滤波
    for(int i = 0; i < arr_size; i++) 
    {
      //cout << " " << i << " " << msg->ranges[i] << endl;
      if(msg->ranges[i] < 0.15) //滤波
      {
        continue;
      }
      if(msg->ranges[i] < smallest_distance_) 
      {
        smallest_distance_ = msg->ranges[i];
        //并计算出角弧度
        alpha_pillar_ = (msg->angle_min + i * msg->angle_increment);
      }
    }
    //cout << "smallest_distance_: " << smallest_distance_ << endl;
    //通过得到的直线距离点，再通过三角函数公式，算出物体在以机器人为原点的X轴和Y轴的偏移量
    x_pillar_ = -smallest_distance_ * cos(alpha_pillar_);
    y_pillar_ = smallest_distance_ * sin(alpha_pillar_);
    std::cout << "x_pillar_ " << x_pillar_ << " y_pillar_ " << y_pillar_ << std::endl;

    //TODO:绕开障碍物逻辑(可以先通过topic模拟障碍物来实现)
    //1、判断障碍物在小车前方的左边/右边
    //2、控制小车后退0.5米
    //3、控制小车旋转45度（根据障碍物在左边/右边，向左/向右旋转45度）
    //4、控制小车前进1.0米
    //5、回到原来的线上继续跟线
    if(is_use_detour_)
    {
      if(x_pillar_ > 0.5 && x_pillar_ <= 1.0 && y_pillar_ > 0.0)
      {
        std::cout << "障碍物在左边" << std::endl;
        ++left_obs_num_;
        std::cout << "left_obs_num_ " << left_obs_num_ << std::endl;
        obstacle_num_ = 1;
      }
      else if(x_pillar_ > 0.5 && x_pillar_ <= 1.0 && y_pillar_ < 0.0)
      {
        std::cout << "障碍物在右边" << std::endl;
        ++right_obs_num_;
        std::cout << "right_obs_num_ " << right_obs_num_ << std::endl;
        obstacle_num_ = 2;
      }
      else if(x_pillar_ > 1.0)
      {
        std::cout << "没有障碍物" << std::endl;
        obstacle_num_ = 0;
      }
    }

    if(smallest_distance_ <= 0.5)//雷达避障的距离
    {
      car_state_ = SCAN_STOP;//小车状态：雷达停车

      car_linear_velocity_ = 0.0; //小车停止
    } 
    else 
    {
      car_linear_velocity_ = 0.5; //小车前进
    }
    smallest_distance_ = 10.0;
  }

  void PurePursuit::simObstacleCallback(const std_msgs::Int32ConstPtr& msg)//仿真障碍物回调
  {
    obstacle_num_ = msg->data;
    //std::cout << "obstacle_num_ " << obstacle_num_ << std::endl;
    //1为障碍物在左边，2为障碍物在右边，0为没有障碍物
    if(obstacle_num_ == 1)
    {
      ++left_obs_num_;
      std::cout << "left_obs_num_ " << left_obs_num_ << std::endl;
    }
    else if(obstacle_num_ == 2)
    {
      ++right_obs_num_;
      std::cout << "right_obs_num_ " << right_obs_num_ << std::endl;
    }
  }

  int PurePursuit::getIndex(double pose_x, double pose_y, double pose_yaw)
  {
    // 方案:通过计算当前坐标和目标轨迹距离，找到一个最小距离的索引号
    int index;
    std::vector<float> best_points;
    for (int i = 0; i < point_num_; i++) 
    {
      float path_x = r_x_[i];
      float path_y = r_y_[i];
      // 遍历所有路径点和当前位置的距离，保存到数组中
      float lad = sqrt(pow(path_x - pose_x, 2) + pow(path_y - pose_y, 2));

      best_points.push_back(lad);
    }
    // 找到数组中最小横向距离
    //std::cout << "数组中最小横向距离 " << *min_element(best_points.begin(), best_points.end()) << std::endl;
    //std::cout << "数组中最小横向距离的索引位置 " << distance(best_points.begin(), min_element(best_points.begin(), best_points.end())) << std::endl;

    index = distance(best_points.begin(), min_element(best_points.begin(), best_points.end()));

    int temp_index;
    for (int i = index; i < point_num_; i++) 
    {
      // 遍历路径点和预瞄点的距离，从最小横向位置的索引开始
      float dis = sqrt(pow(r_y_[index] - r_y_[i], 2) + pow(r_x_[index] - r_x_[i], 2));
      // 判断跟预瞄点的距离
      if (dis < preview_dis_) 
      {
        temp_index = i;
      } 
      else 
      {
        break;
      }
    }
    index = temp_index;
    //std::cout << "index " << index << std::endl;

    return index;
  }

  inline double PurePursuit::thetaLimit(double yaw)//角度限制 0~180度，-180度~0
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

  double PurePursuit::Xianfu(double value, double Amplitude)//限制最大速度幅度
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
  double PurePursuit::Incremental_PI(double Velocity, double Target)
  {   
    static double Bias, Speed, Last_bias, Integral_bias;
    Bias = Target - Velocity;//计算偏差
    Integral_bias += Bias;
    Integral_bias = Xianfu(Integral_bias, 1.0);//速度最大限制在0.8
    Speed = Velocity_KP * Bias + Velocity_KI * Integral_bias + Velocity_KD *(Bias - Last_bias);//增量式PI控制器
    Last_bias = Bias;//保存上一次偏差 

    if(Speed > 1.0)
      Speed = 1.0;
    if(Speed < -1.0)
      Speed = -1.0;

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
  double PurePursuit::Position_PID(double Position, double target)
  {   
    static float Bias, Speed, Integral_bias, Last_Bias;
    Bias = target - Position;//计算偏差
    Integral_bias += Bias;//求出偏差的积分
    Integral_bias = Xianfu(Integral_bias, 1.0);
    Speed = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);//位置式PID控制器
    Last_Bias = Bias;//保存上一次偏差 
    
    if(Speed > 1.0)
      Speed = 1.0;
    if(Speed < -1.0)
      Speed = -1.0;

    return Speed;//增量输出
  }

  void PurePursuit::circumventLeft()//向左绕障
  {
    geometry_msgs::Twist vel_msg;//发布速度
    if(is_record_pose_)
    {
      record_pose_x_ = current_pose_x_;
      record_pose_y_ = current_pose_y_;
      record_pose_yaw_ = current_pose_yaw_;
      
      is_record_pose_ = false;
    }
    double dis_record = sqrt(pow(record_pose_y_ - current_pose_y_, 2) + pow(record_pose_x_ - current_pose_x_, 2));
    //std::cout << "dis_record " << dis_record << std::endl;

    if(dis_record <= 0.5 && !is_back_finish_)
    {
      //std::cout << "小车绕障动作：小车后退" << std::endl;
      car_detour_ = BACK;

      vel_msg.linear.x = -0.2;//后退
      vel_msg.angular.z = 0.0;
      cmd_vel_pub_.publish(vel_msg);
    }
    else
    {
      is_back_finish_ = true;

      double dis_yaw = thetaLimit(record_pose_yaw_ - current_pose_yaw_);
      //std::cout << "dis_yaw " << dis_yaw << std::endl;//0~180度，-180度~0

      if(dis_yaw >= -45.0 * M_PI / 180.0  && !is_turn_finish_)
      {
        //std::cout << "小车绕障动作：小车旋转" << std::endl;
        car_detour_ = SPIN;

        vel_msg.linear.x = 0.0;//旋转
        vel_msg.angular.z = 0.4;
        cmd_vel_pub_.publish(vel_msg); 
      }    
      else
      {
        is_turn_finish_ = true;

        if(is_distance_flag_)
        {
          distance_pose_x_ = current_pose_x_;
          distance_pose_y_ = current_pose_y_;

          is_distance_flag_ = false;
        }
        double dis_distance = sqrt(pow(distance_pose_y_ - current_pose_y_, 2) + pow(distance_pose_x_ - current_pose_x_, 2));
        //std::cout << "dis_distance " << dis_distance << std::endl;

        if(dis_distance <= 1.0 && is_back_finish_ && is_turn_finish_)
        {
          //std::cout << "小车绕障动作：小车前进" << std::endl;
          car_detour_ = ADVANCE;

          vel_msg.linear.x = 0.4;//前进
          vel_msg.angular.z = 0.0;
          cmd_vel_pub_.publish(vel_msg);
        } 
        else
        {
          //std::cout << "小车绕障动作：完成绕障" << std::endl;
          car_detour_ = FINISH;

          obstacle_num_ = 0;
          is_distance_flag_ = true;
          is_record_pose_ = true;
          is_back_finish_ = false;
          is_turn_finish_ = false;
          left_obs_num_ = 0;
          right_obs_num_ = 0;
        }         
      }       
    }
  }

  void PurePursuit::circumventRight()//向右绕障
  {
    geometry_msgs::Twist vel_msg;//发布速度
    if(is_record_pose_)
    {
      record_pose_x_ = current_pose_x_;
      record_pose_y_ = current_pose_y_;
      record_pose_yaw_ = current_pose_yaw_;
      
      is_record_pose_ = false;
    }
    double dis_record = sqrt(pow(record_pose_y_ - current_pose_y_, 2) + pow(record_pose_x_ - current_pose_x_, 2));
    //std::cout << "dis_record " << dis_record << std::endl;

    if(dis_record <= 0.5 && !is_back_finish_)
    {
      //std::cout << "小车绕障动作：小车后退" << std::endl;
      car_detour_ = BACK;

      vel_msg.linear.x = -0.2;//后退
      vel_msg.angular.z = 0.0;
      cmd_vel_pub_.publish(vel_msg);
    }
    else
    {
      is_back_finish_ = true;

      double dis_yaw = thetaLimit(record_pose_yaw_ - current_pose_yaw_);
      //std::cout << "dis_yaw " << dis_yaw << std::endl;//0~180度，-180度~0

      if(dis_yaw <= 45.0 * M_PI / 180.0  && !is_turn_finish_)
      {
        //std::cout << "小车绕障动作：小车旋转" << std::endl;
        car_detour_ = SPIN;

        vel_msg.linear.x = 0.0;//旋转
        vel_msg.angular.z = -0.4;
        cmd_vel_pub_.publish(vel_msg); 
      }    
      else
      {
        is_turn_finish_ = true;

        if(is_distance_flag_)
        {
          distance_pose_x_ = current_pose_x_;
          distance_pose_y_ = current_pose_y_;

          is_distance_flag_ = false;
        }
        double dis_distance = sqrt(pow(distance_pose_y_ - current_pose_y_, 2) + pow(distance_pose_x_ - current_pose_x_, 2));
        //std::cout << "dis_distance " << dis_distance << std::endl;

        if(dis_distance <= 1.0 && is_back_finish_ && is_turn_finish_)
        {
          //std::cout << "小车绕障动作：小车前进" << std::endl;
          car_detour_ = ADVANCE;

          vel_msg.linear.x = 0.4;//前进
          vel_msg.angular.z = 0.0;
          cmd_vel_pub_.publish(vel_msg);
        } 
        else
        {
          //std::cout << "小车绕障动作：完成绕障" << std::endl;
          car_detour_ = FINISH;

          obstacle_num_ = 0;
          is_distance_flag_ = true;
          is_record_pose_ = true;
          is_back_finish_ = false;
          is_turn_finish_ = false;
          left_obs_num_ = 0;
          right_obs_num_ = 0; 
        }         
      }       
    }
  }

  void PurePursuit::run()
  {
    int index = getIndex(current_pose_x_, current_pose_y_, current_pose_yaw_);
    //std::cout << "index " << index << std::endl;

    double alpha = thetaLimit(atan2(r_y_[index] - current_pose_y_, r_x_[index] - current_pose_x_) - current_pose_yaw_);
    //std::cout << "alpha " << alpha << std::endl;//0~180度，-180度~0

    // 当前点和目标点的距离Id
    double dl = sqrt(pow(r_y_[index] - current_pose_y_, 2) + pow(r_x_[index] - current_pose_x_, 2));
    std::cout << "dl " << dl << std::endl;

    double curvature_k = 2 * sin(alpha) / dl;//跟踪曲率 k = 2 * sin(a) / Ld
    //std::cout << "curvature_k " << curvature_k << std::endl;

    double dis_pos = sqrt(pow(r_y_.back() - current_pose_y_, 2) + pow(r_x_.back() - current_pose_x_, 2));//距离终点的距离
    //std::cout << "dis_pos " << dis_pos << std::endl;

    double path_range = sqrt(pow(r_y_[index] - current_pose_y_, 2) + pow(r_x_[index] - current_pose_x_, 2));//路径范围，给判断是否纠正线路用
    //std::cout << "path_range " << path_range << std::endl;

    if((alpha >= 10.0 * M_PI / 180.0 || alpha <= -10.0 * M_PI / 180.0) && obstacle_num_ == 0 && path_range <= 0.5)//纠正线路
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
      if(dl > 0.12 || index != (point_num_-1)) 
      {
        car_state_ = PURE_PURSUIT;//小车状态：纯跟踪跟线

        geometry_msgs::Twist vel_msg;//发布速度
        if(is_use_pid_param_)//使用PID调速
        {
          //增加仿真障碍物绕障逻辑
          if(obstacle_num_ == 0)//正常行驶，无障碍物
          {
            //增加串级PID控制线速度
            double motor = Position_PID(-dis_pos, 0.0);//小车当前位置到目标位置的距离，距离逐渐增加
            vel_msg.linear.x = -Incremental_PI(current_velocity_.linear.x, motor);
            std::cout << "小车线速度 " << vel_msg.linear.x << std::endl;
            vel_msg.angular.z = theta;
            cmd_vel_pub_.publish(vel_msg);
          } 
          else if(obstacle_num_ == 1 || right_obs_num_ >= 10)//左边有障碍物，或者右边累计10次都识别到障碍物，向右绕障
          {
            car_state_ = CIRCUMVENT;//小车状态：绕障

            circumventRight();//向右绕障
          }   
          else if(obstacle_num_ == 2 || left_obs_num_ >= 10)//右边有障碍物，或者左边累计10次都识别到障碍物，向左绕障
          {
            car_state_ = CIRCUMVENT;//小车状态：绕障

            circumventLeft();//向左绕障
          } 
        }
        else//不使用PID调速
        {
          //增加仿真障碍物绕障逻辑
          if(obstacle_num_ == 0)//正常行驶，无障碍物
          {
            vel_msg.linear.x = car_linear_velocity_;
            vel_msg.angular.z = theta;
            cmd_vel_pub_.publish(vel_msg);
          } 
          else if(obstacle_num_ == 1 || right_obs_num_ >= 3)//左边有障碍物，或者右边累计10次都识别到障碍物，向右绕障
          {
            car_state_ = CIRCUMVENT;//小车状态：绕障

            circumventRight();//向右绕障
          }   
          else if(obstacle_num_ == 2 || left_obs_num_ >= 3)//右边有障碍物，或者左边累计10次都识别到障碍物，向左绕障
          {
            car_state_ = CIRCUMVENT;//小车状态：绕障

            circumventLeft();//向左绕障
          } 
        }
      } 
      else 
      {
        car_state_ = CAR_STOP;//小车状态：正常停车

        geometry_msgs::Twist vel_msg;
        vel_msg.linear.x = 0;
        vel_msg.angular.z = 0;
        cmd_vel_pub_.publish(vel_msg);

        is_global_path_sub_ = false;
      }
    }
  }

  void PurePursuit::MainLoop()
  { 
    ros::Rate loop_rate(20);

    while (ros::ok())
    {  
      if(is_global_path_sub_ && is_current_pose_sub_)
      {
        run();

        if(obstacle_num_ == 0)
        {
          switch(car_state_)//主状态机
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
            case CIRCUMVENT:
              std::cout << "小车状态：绕障" << std::endl;
              break; 
            default:
              break;
          }
        }
        else if(obstacle_num_ == 1 || obstacle_num_ == 2)
        {
          switch(car_detour_)//绕障子状态机
          {
            case BACK:
              std::cout << "绕障状态：小车后退" << std::endl;
              break;
            case SPIN:
              std::cout << "绕障状态：小车旋转" << std::endl;
              break;
            case ADVANCE:
              std::cout << "绕障状态：小车前进" << std::endl;
              break; 
            case FINISH:
              std::cout << "绕障状态：完成绕障" << std::endl;
              break; 
            default:
              break;      
          }
        }

        is_current_pose_sub_ = false;
      } 

      ros::spinOnce();
      loop_rate.sleep();
    }
  }

}

