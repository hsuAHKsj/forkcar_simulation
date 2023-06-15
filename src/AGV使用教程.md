整体分为两套程序：
1、纯跟踪规划，可以走曲线
2、点到点规划，只能走直线，带加减速（重点维护这套程序）

代码架构：
check_point 录制点
control 控制相关
obstacle_avoidance 雷达避障
point2point 点到点规划（重点）
pub_global_path 发布线路
pub_point 发布点
pure_pursuit 纯跟踪，可以走曲线
record_path 录制线

仿真测试：
一、跑纯跟踪规划
启动仿真（如果更改地图，将地图文件替换到agv_robot/maps中的map.pgm和map.yaml）:
$ roslaunch agv_robot sim_navigation.launch 

1、启动录制线路（录制完成关闭程序）:
$ rosrun agv_robot record_path

发布录制第几条线路（数字要为正数）:
$ rostopic pub -1 /record_path_number std_msgs/Int32 "data: 1" 
启动键盘遥控程序:
$ rosrun teleop_twist_keyboard teleop_twist_keyboard.py 

2、启动纯跟踪程序（可选择是否使用PID控制加减速）:
$ roslaunch agv_robot pure_pursuit.launch

启动发布线路程序（可选择是否循环绕圈）:
$ roslaunch agv_robot pub_global_path.launch

发布第几条线路:
$ rostopic pub -1 /set_path_number std_msgs/Int32 "data: 1"

发布仿真障碍物（1为左边有障碍物，2为右边有障碍物）：
$ rostopic pub -1 /sim_obstacle std_msgs/Int32 "data: 1"

二、跑点到点规划（需要两个点以上）
启动仿真（如果更改地图，将地图文件替换到agv_robot/maps中的map.pgm和map.yaml）:
$ roslaunch agv_robot sim_navigation.launch 

1、启动录制线路（录制完成关闭程序）:
$ rosrun agv_robot check_point
按键盘'a'键，再按'Enter'键可以录制点

启动键盘遥控程序:
$ rosrun teleop_twist_keyboard teleop_twist_keyboard.py

2、启动点到点程序（可选择是否循环绕圈）:
$ roslaunch agv_robot point2point.launch

启动发布点的程序:
$ rosrun agv_robot pub_point

启动记录到达对接点次数的程序:
$ rosrun agv_robot control

启动科力雷达：
$ roslaunch sdkeli_ls_udp sdkeli_ls1207de_udp_nodelet_with_1_lidar.launch











