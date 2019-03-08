#include<iostream>
#include<ros/ros.h>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Pose.h>
#include<sstream>
#include<fstream>
#include<math.h>
#include<turtlesim/Pose.h>
#define tolx 0.1
#define tolz 0.12
#include<sstream>
using namespace std;
ros::Publisher velocity_publisher;
ros::Subscriber subscriber_pose;
geometry_msgs::Twist vel_msg;
turtlesim::Pose CurPose;												

struct coordi
{
	double x,y;
};

double degree_to_rad(double angle_in_degrees)
{
	double ans= angle_in_degrees*3.141592/180;
	return ans;
}
void pose_callback(const turtlesim::Pose& msgIn){

	CurPose.x = msgIn.x;
	CurPose.y = msgIn.y;
	CurPose.theta = msgIn.theta;	
}
float tolangle(turtlesim::Pose curpose, coordi despose)
{
	float Ex = despose.x - curpose.x;									
	float Ey = despose.y - curpose.y;									
	
	float dest = atan2f(Ey, Ex); 										

	float Et = dest - curpose.theta;
	
	return Et;

}
float tolmove(turtlesim::Pose curpose, coordi despose)
{
	float Ex = despose.x - curpose.x;			
	float Ey = despose.y - curpose.y;						
	float Et = tolangle(curpose, despose);							
	
	float Etx = sqrt(Ex*Ex + Ey*Ey);
	
	return Etx;
}

/*void move(double speed=2, double distance=1, int isForward=1)
{

	
	if(speed<0)
		speed=-speed;
	if(isForward==1)
		{vel_msg.linear.x=speed;
		}
	else
		{vel_msg.linear.x=-speed;
			}

		vel_msg.linear.y=0;
		vel_msg.linear.z=0;
		
		vel_msg.angular.x=0;
		vel_msg.angular.y=0;
		vel_msg.angular.z=0;
		double t0= ros::Time::now().toSec();
		double current_distance=0;

	ros::Rate loop_rate(10);
	ROS_INFO("[Moving] linear.x =%.2f, angular.z=%.2f\n",vel_msg.linear.x,vel_msg.angular.z);

	do{

		velocity_publisher.publish(vel_msg);
		double t1= ros::Time::now().toSec();
		current_distance= speed*(t1-t0);
			ros::spinOnce();
		loop_rate.sleep();
		ros::spinOnce();

	}while(current_distance<distance);
	vel_msg.linear.x=0;
	velocity_publisher.publish(vel_msg);	

}*/
/*void rotate(double angular_speed= 3.141592/18, double relative_angle = 0.75, int clockwise=-1)
{

		vel_msg.linear.x=0;
		vel_msg.linear.y=0;
		vel_msg.linear.z=0;
		
		vel_msg.angular.x=0;
		vel_msg.angular.y=0;

		if(angular_speed<0)
			angular_speed=-angular_speed;
		if(clockwise==1)
				vel_msg.angular.z=-angular_speed;
			else
				vel_msg.angular.z=angular_speed;
		double t0= ros::Time::now().toSec();
		double current_angle=0;
	ROS_INFO("[Rotating] linear.x =%.2f, angular.z=%.2f\n",vel_msg.linear.x,vel_msg.angular.z);

	ros::Rate loop_rate(10);
	
	do{

		velocity_publisher.publish(vel_msg);

		double t1= ros::Time::now().toSec();
		current_angle= (angular_speed)*(t1-t0);
		loop_rate.sleep();
		ros::spinOnce();

	}while(current_angle<=relative_angle);
	vel_msg.angular.z=0;
	velocity_publisher.publish(vel_msg);	
	//ros::spin();

} */
int main(int argc, char **argv)
{
	ifstream fin("/home/sahiljindal/IP/nodes.txt");
	ros::init(argc,argv,"pubTsim");
	ros::NodeHandle n;
	velocity_publisher= n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel",1000);
	subscriber_pose=n.subscribe("/turtle1/pose",1000,pose_callback);
	ros::Rate loop_rate(10);
	coordi dest;
	int count=0;
	string line;
	float x[40],z[40],errx=0,errz=0;
	int q;
	int totalpoints=0;
	ROS_INFO("Going to start position!!\n");
	while(getline(fin,line))
	{		int x1,z1;
		    stringstream geek(line);
		    geek>>x1;
		    getline(fin,line);
		    stringstream geek2(line);
		    geek2>>z1;
		    x[totalpoints]=x1/40.0;

		    z[totalpoints]=z1/40.0;
		    ++totalpoints;		    
	}
	/*cout<<"\nTotal points:"<<totalpoints<<":";
	for(int i=0;i<totalpoints;++i)
	{
		cout<<"x: "<<x[i]<<"\tz: "<<z[i]<<"\n";
	}*/
	
	while(ros::ok()&& n.ok()){
	
	ros::spinOnce();
	dest.x=x[count];
	dest.y=z[count];
	errx = tolmove(CurPose, dest);					
	errz= tolangle(CurPose, dest);	
	cout<<"\nError in x: "<<errx<<"\t Error in angle: "<<errz;


	if(errz*errz>tolz*tolz)

		{
			vel_msg.angular.z = 0.7* errz;							
			vel_msg.linear.x = 0;
		}
	else 	
		{
			vel_msg.angular.z = 0;							
			if (errx>tolx)										
				vel_msg.linear.x = 0.8 * errx;							
			else
				{	vel_msg.linear.x = 0;
					++count;	
					if(count==totalpoints)
					{
						cout<<"Finished";
						break;
					}
				}
		}
	cout<<"\nvel in x: "<<vel_msg.linear.x <<"\t vel in angle: "<<vel_msg.angular.z;

	velocity_publisher.publish(vel_msg);
	loop_rate.sleep();	
	}
	fin.close();
	return 0;
}
