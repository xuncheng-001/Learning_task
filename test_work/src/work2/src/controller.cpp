//
// Created by xuncheng on 25-2-18.
//
#include <boost/unordered/unordered_map.hpp>
#include <controller_interface/controller.h>
#include <hardware_interface/actuator_command_interface.h>
#include <hardware_interface/joint_command_interface.h>
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <random>
#include <boost/algorithm/string/join.hpp>
#include <control_toolbox/pid.h>
#include <pluginlib/class_list_macros.h>

namespace work2
{
    class FeedforwardController : public controller_interface::Controller<hardware_interface::EffortJointInterface>
    {
    public:
        ros::NodeHandle nh_;
        FeedforwardController()
        {
            //随机获得计算大能量机关所需要的参数
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.780, 1.045);
            a= dis(gen);
            std::uniform_real_distribution<> dis2(1.884, 2.000);
            w= dis2(gen);
            b=2.090-a;

            //pid参数获取
            double p;
            nh_.getParam("/link2_controller/pid/p",p );
            double i;
            nh_.getParam("/link2_controller/pid/i",i);
            double d;
            nh_.getParam("/link2_controller/pid/d",d);
            double i_clamp;
            nh_.getParam("/link2_controller/pid/i_max",i_clamp);

            pid_.setGains(p ,i , d , i_clamp ,0.0,false);

            //命令发布话题
            controller_pub_ = nh_.advertise<std_msgs::Float64>("controller_command", 10);
            //发布期望速度
            target = nh_.advertise<std_msgs::Float64>("target_command", 10);
            //决定是否要使用前馈
            Feed_yes_or_not = nh_.advertise<std_msgs::Float64>("feed_yes_or_not", 10);


        }
        //回调函数，接受controller_command 的话题发布，来决定风车模式
        void callback(const std_msgs::Float64ConstPtr& msg)
        {
            check=msg->data;
        }
        //回调函数，决定是否使用前馈
        void feedcallback(const std_msgs::Float64ConstPtr& msg)
        {
            if (msg->data==1)
            {
                F=1.0;
            }
            else
            {
                F=0;
            }
        }
        bool init(hardware_interface::EffortJointInterface* hw, ros::NodeHandle& node_handle) override
        {
            //获取关节句柄
            joint_= hw->getHandle("link2_joint");
            return true;
        }
        void update(const ros::Time& time, const ros::Duration& period)override
        {
            //命令接收话题，用于决定使用大风车模式还是小风车模式
            check_subscriber_ = nh_.subscribe("controller_command",10 , &FeedforwardController::callback ,this);
            Feed = nh_.subscribe("feed_yes_or_not", 10, &FeedforwardController::feedcallback ,this);
            //更新t变量
            t=time.toSec();
            //风车模式选择的实现
            if (check==2)
            {
                //大能量机关的速度目标函数
                desired_speed =a*sin(w*t)+b;
                f_desired_speed= a*sin(w*(t-0.1))+b;
            }
            else if (check==1)
            {
                //小能量机关的固定速度
                desired_speed=1.02;
            }
            else
            {
                desired_speed=0.0;
            }
            //反馈要发的命令
            double B_command = pid_.computeCommand(desired_speed - joint_.getVelocity(), period);
            //前馈要发的命令
            double F_command = F*(desired_speed- f_desired_speed);
            //所要发的命令汇总
            msg.data =F_command + B_command;
            joint_.setCommand(msg.data);
            //发布期望速度
            target_command.data =desired_speed;
            target.publish(target_command);
        }

    private:
        hardware_interface::JointHandle joint_;
        ros::Publisher controller_pub_;
        ros::Subscriber check_subscriber_;
        std_msgs::Float64 msg;
        std_msgs::Float64 lmsg;
        double desired_speed;
        double f_desired_speed;
        double F;
        double t=0;
        double a;
        double b;
        double w;
        double check;
        control_toolbox::Pid pid_;
        ros::Publisher target;
        ros::Publisher Feed_yes_or_not;
        ros::Subscriber Feed;
        std_msgs::Float64 target_command;

        //初始化随机数种子
        std::random_device rd;
    };

}
PLUGINLIB_EXPORT_CLASS(work2::FeedforwardController, controller_interface::ControllerBase)