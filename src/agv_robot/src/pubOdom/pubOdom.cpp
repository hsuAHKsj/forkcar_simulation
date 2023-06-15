#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <fstream>
#include <iostream>
#include <cmath>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "tf_Pose_Publisher");

    ros::NodeHandle node;
    ros::Publisher odom_pub = node.advertise<nav_msgs::Odometry>("/odom", 60);
    tf::StampedTransform transform;
    tf::TransformListener listener;
    ros::Rate rate(10.0);

    while (ros::ok())
    {
        ros::Time start = ros::Time::now();
        std::cout << "StartTime:" << start << std::endl;
        tf::StampedTransform transform;
        try
        {
            // 得到坐标系 map 和坐标系 base_link 之间的关系
            listener.waitForTransform("map", "base_link", ros::Time(0), ros::Duration(3.0));
            listener.lookupTransform("map", "base_link", ros::Time(0), transform);
        }
        catch (tf::TransformException &ex)
        {
            ROS_ERROR("%s", ex.what());
            ros::Duration(1.0).sleep();
        }

        nav_msgs::Odometry odom;
        odom.header.stamp = ros::Time::now();
        odom.header.frame_id = "map";
        odom.child_frame_id = "base_link";

        odom.pose.pose.position.x = transform.getOrigin().x();
        odom.pose.pose.position.y = transform.getOrigin().y();
        odom.pose.pose.position.z = transform.getOrigin().z();

        tf::Quaternion q = transform.getRotation();
        odom.pose.pose.orientation.x = q.x();
        odom.pose.pose.orientation.y = q.y();
        odom.pose.pose.orientation.z = q.z();
        odom.pose.pose.orientation.w = q.w();

        odom_pub.publish(odom);

        printf("x: %f, y: %f, z: %f, qx: %f, qy: %f, qz: %f, qw: %f\n",
               odom.pose.pose.position.x, odom.pose.pose.position.y, odom.pose.pose.position.z,
               odom.pose.pose.orientation.x, odom.pose.pose.orientation.y,
               odom.pose.pose.orientation.z, odom.pose.pose.orientation.w);

        rate.sleep();

        ros::Time end = ros::Time::now();
        std::cout << "EndTime:" << end << std::endl;
    }

    return 0;
}
