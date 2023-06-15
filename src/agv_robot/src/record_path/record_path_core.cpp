#include "agv_robot/record_path.h"

namespace RecordPathNS
{
    RecordPath::RecordPath() : is_number_sub_(false), is_record_(false), is_pose_sub_(false), receive_once_(false)
    {
        odom_sub_ = nh_.subscribe("odom", 10, &RecordPath::odomCallback, this); // 里程计
        number_sub_ = nh_.subscribe("/record_path_number", 1, &RecordPath::numberCallback, this);
    }

    RecordPath::~RecordPath()
    {
        outFile_.close();
    }

    void RecordPath::numberCallback(const std_msgs::Int32ConstPtr &msg)
    {
        current_path_number_ = msg->data;
        std::cout << "current_path_number_ " << current_path_number_ << std::endl;
        if (last_path_num_ != current_path_number_)
        {
            outFile_.close();

            is_record_ = false;
        }
        is_number_sub_ = true;

        if (current_path_number_ > 0 && is_number_sub_)
        {
            number_ = current_path_number_;

            std::string str1 = "/home/nvidia/Desktop/BL_fork/src/path/path_";
            std::string str2 = std::to_string(number_);
            std::string str3 = ".csv";
            std::string str4 = str1 + str2 + str3;

            std::cout << str4 << std::endl;

            std::cout << "开始录制线路：" << number_ << std::endl;

            outFile_.open(str4, std::ios::app);

            if (!outFile_)
            {
                std::cout << "打开文件失败！" << std::endl;
                exit(1);
            }
            std::cout << "请移动机器人录制线路" << std::endl;

            path_num_ = 0;
            receive_once_ = false;
            is_number_sub_ = false;
            is_record_ = true;
        }
        last_path_num_ = current_path_number_;
        std::cout << "last_path_num_ " << last_path_num_ << std::endl;
    }

    void RecordPath::odomCallback(const nav_msgs::OdometryConstPtr &msg) // 回调当前位置
    {
        current_pos_x_ = msg->pose.pose.position.x;
        current_pos_y_ = msg->pose.pose.position.y;
        current_pos_yaw_ = tf::getYaw(msg->pose.pose.orientation);

        is_pose_sub_ = true;
    }

    void RecordPath::run()
    {
        if(!receive_once_)//first subscribe 接收第一个数据
        {
            //位置信息以四元数的形式存储起来
            outFile_ << current_pos_x_ << "," << current_pos_y_ << ","  << current_pos_yaw_ << std::endl;
            std::cout << "写入初始位置信息" << std::endl;

            receive_once_ = true;

            pre_pos_x_ = current_pos_x_;//记录当前位置
            pre_pos_y_ = current_pos_y_;
            pre_pos_yaw_ = current_pos_yaw_;

            std::cout << "pre_pose_x_ " << pre_pos_x_;
            std::cout << " pre_pose_y_ " << pre_pos_y_;
            std::cout << " pre_pose_yaw_ " << pre_pos_yaw_ << std::endl;
        }
        else
        {
            //计算这个距离数据，勾股定理计算
            float distance = sqrt(pow((current_pos_x_ - pre_pos_x_), 2) +
                                  pow((current_pos_y_ - pre_pos_y_), 2));
            //如果小车移动了0.5米
            if(distance >= 0.5)
            {
                ++path_num_;
                //写入需要录取的数据
                outFile_ << current_pos_x_ << "," << current_pos_y_ << "," << current_pos_yaw_ << std::endl;
                std::cout << "正在进行第 " << path_num_ + 1 << " 个数据写入......" << std::endl;

                pre_pos_x_ = current_pos_x_;//更新当前位置
                pre_pos_y_ = current_pos_y_;
                pre_pos_yaw_ = current_pos_yaw_;
                std::cout << "current_pos_x_ " << pre_pos_x_;
                std::cout << " current_pos_y_ " << pre_pos_y_;
                std::cout << " current_pos_yaw_ " << pre_pos_yaw_ << std::endl;
            }
        }
    }

    void RecordPath::MainLoop()
    {
        ros::Rate loop_rate(20);

        while (ros::ok())
        {
            // std::cout << "is_record_:" << is_record_ << ", is_pose_sub_:" << is_pose_sub_<< std::endl;
            if(is_record_ && is_pose_sub_)
            {
                run();

                is_pose_sub_ = false;
            }

            ros::spinOnce();
            loop_rate.sleep();
        }
    }
}
